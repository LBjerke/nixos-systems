#include "dsp/resampler.hpp"
#include "dsp/fir.hpp"
#include "../pffft/pffft.h"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <atomic>

#define FS 2048
#define AC 2.0*M_PI/FS
#define FS2 1024
#define NF 256
#define IFS 1.0f/FS
#define IFS2 1.0f/FS2
#define IM_PI 1.0f/M_PI

using namespace std;

using simd::float_4;

struct wtFrame {
  vector<float> sample;
  vector<float> magnitude;
  vector<float> phase;
  bool morphed=false;
  bool used=false;

  wtFrame() {
    sample.resize(FS,0);
    magnitude.resize(FS2,0);
    phase.resize(FS2,0);
  }

  void calcFFT();
  void calcIFFT();
  void calcWav();
  void normalize();
  void smooth();
  void window();
  void removeDCOffset();
  void loadSample(size_t sCount, bool interpolate, float *wav);
  void loadMagnitude(size_t sCount, bool interpolate, float *magn);
  float maxAmp();
  void gain(float g);
  void reset();
};

inline void wtFrame::reset() {
  for(size_t i=0; i<FS2; i++) {
    sample[i]=0.0f;
    magnitude[i]=0.0f;
    phase[i]=0.0f;
  }
  for(size_t i=FS2; i<FS; i++) {
    sample[i]=0.0f;
  }
  used=false;
  morphed=false;
}

void wtFrame::calcFFT() {
  PFFFT_Setup *pffftSetup;
	pffftSetup = pffft_new_setup(FS, PFFFT_REAL);
	float *fftIn;
	float *fftOut;
	fftIn = (float*)pffft_aligned_malloc(FS*sizeof(float));
	fftOut = (float*)pffft_aligned_malloc(FS*sizeof(float));
	memset(fftIn, 0, FS*sizeof(float));
	memset(fftOut, 0, FS*sizeof(float));

	for (size_t k = 0; k < FS; k++) {
		fftIn[k] = sample[k];
	}

	pffft_transform_ordered(pffftSetup, fftIn, fftOut, 0, PFFFT_FORWARD);

	for (size_t k = 0; k < FS2; k++) {
		if ((abs(fftOut[2*k])>1e-2f) || (abs(fftOut[2*k+1])>1e-2f)) {
			float real = fftOut[2*k];
			float imag = fftOut[2*k+1];
			phase[k] = atan2(imag,real);
			magnitude[k] = 2.0f*sqrt(real*real+imag*imag)/FS;
		}
    else {
      phase[k] = 0.0f;
			magnitude[k] = 0.0f;
    }
	}

	pffft_destroy_setup(pffftSetup);
	pffft_aligned_free(fftIn);
	pffft_aligned_free(fftOut);
}

void wtFrame::calcIFFT() {
  PFFFT_Setup *pffftSetup;
	pffftSetup = pffft_new_setup(FS, PFFFT_REAL);
	float *fftIn;
	float *fftOut;
	fftIn = (float*)pffft_aligned_malloc(FS*sizeof(float));
	fftOut =  (float*)pffft_aligned_malloc(FS*sizeof(float));
	memset(fftIn, 0, FS*sizeof(float));
	memset(fftOut, 0, FS*sizeof(float));

	for (size_t i = 0; i < FS2; i++) {
		fftIn[2*i] = magnitude[i]*cos(phase[i]);
		fftIn[2*i+1] = magnitude[i]*sin(phase[i]);
	}

	pffft_transform_ordered(pffftSetup, fftIn, fftOut, 0, PFFFT_BACKWARD);

	for (size_t i = 0; i < FS; i++) {
		sample[i]=fftOut[i]*0.5f;
	}

	pffft_destroy_setup(pffftSetup);
	pffft_aligned_free(fftIn);
	pffft_aligned_free(fftOut);
}

void wtFrame::calcWav() {
  for(size_t i = 0 ; i < FS; i++) {
    sample[i] = 0;
		for(size_t j = 0; j < FS2; j++) {
			if (magnitude[j]>0) {
        sample[i] += magnitude[j] * cos(i*j*AC + phase[j]);
      }
		}
  }
}

inline void wtFrame::normalize() {
  float amp = maxAmp();
  float g= amp>0?1.0f/amp:0.0f;
	gain(g);
}

inline void wtFrame::smooth() {
  for(size_t i=0; i<16;i++) {
    float avg=(sample[i]+sample[FS-i-1])/2.0f;
    sample[i]= ((16-i)*avg+i*sample[i])/16.0f;
    sample[FS-i-1]= ((16-i)*avg+i*sample[FS-i-1])/16.0f;
  }
}

inline void wtFrame::window() {
  for (size_t i = 0; i < FS;i++) {
		float window = std::min((float)(-10.0f * cos(2.0f * M_PI * (float)i / FS) + 10.0f), 1.0f);
		sample[i] *= window;
	}
}

void wtFrame::removeDCOffset() {
  calcFFT();
  magnitude[0]=0.0f;
  calcIFFT();
}

void wtFrame::loadSample(size_t sCount, bool interpolate, float *wav) {
  if (interpolate) {
    for(size_t i=0;i<FS;i++) {
      size_t index = i*((float)std::max(sCount-1,(size_t)0)/(float)FS);
      float pos = (float)i*((float)std::max(sCount-1,(size_t)0)/(float)FS);
      sample[i]=rescale(pos,index,index+1,*(wav+index),*(wav+index+1));
    }
  }
  else {
    for(size_t i=0;i<FS;i++) {
      if (i<sCount) {
        sample[i]=*(wav+i);
      }
      else {
        sample[i]=0.0f;
      }
    }
  }
}

inline float wtFrame::maxAmp() {
  float amp = 0.0f;
	for(size_t i = 0 ; i < FS; i++) {
		amp = max(amp,abs(sample[i]));
	}
	return amp;
}

inline void wtFrame::gain(float g) {
	for(size_t i = 0 ; i < FS; i++) {
		sample[i]*=g;
	}
}

struct wtTable {
  std::vector<wtFrame> frames;
  size_t nFrames=0;

  wtTable() {
    for(size_t i=0; i<NF; i++) {
      wtFrame frame;
      frames.push_back(frame);
    }
  }

  void loadSample(size_t sCount, size_t frameSize, bool interpolate, float *sample);
  void loadMagnitude(size_t sCount, size_t frameSize, bool interpolate, float *magn);
  void normalize();
  void normalizeFrame(size_t index);
  void normalizeAllFrames();
  void smooth();
  void smoothFrame(size_t index);
  void window();
  void windowFrame(size_t index);
  void removeDCOffset();
  void calcFFT();
  void removeFrameDCOffset(size_t index);
  void addFrame(size_t index);
  void removeFrame(size_t index);
  void morphFrames();
  void morphSpectrum();
  void morphSpectrumConstantPhase();
  void reset();
  void deleteMorphing();
  void init();
  void copyFrame(size_t from, size_t to);
};

void wtTable::copyFrame(size_t from, size_t to) {
  for(size_t i=0; i<FS2; i++) {
    frames[to].sample[i]=frames[from].sample[i];
    frames[to].magnitude[i]=frames[from].magnitude[i];
    frames[to].phase[i]=frames[from].phase[i];
  }
  for(size_t i=FS2; i<FS; i++) {
    frames[to].sample[i]=frames[from].sample[i];
  }
}

void wtTable::loadSample(size_t sCount, size_t frameSize, bool interpolate, float *sample) {
  reset();
  size_t sUsed=0;
  while ((sUsed != sCount) && (nFrames<NF)) {
    size_t lenFrame = std::min(frameSize,sCount-sUsed);
    frames[nFrames].loadSample(lenFrame,interpolate,sample+sUsed);
    sUsed+=lenFrame;
    nFrames++;
  }
}

void wtTable::normalize() {
  float amp = 0.0f;
  for(size_t i=0; i<nFrames; i++) {
    amp = max(amp,frames[i].maxAmp());
  }
  float g=amp>0?1.0f/amp:0.0f;
  for(size_t i=0; i<nFrames; i++) {
    frames[i].gain(g);
  }
}

inline void wtTable::normalizeFrame(size_t index) {
  frames[index].normalize();
}

inline void wtTable::normalizeAllFrames() {
  for(size_t i=0; i<nFrames; i++) {
    frames[i].normalize();
  }
}

inline void wtTable::smooth() {
  for(size_t i=0; i<nFrames;i++) {
    frames[i].smooth();
  }
}

inline void wtTable::smoothFrame(size_t index) {
  frames[index].smooth();
}

inline void wtTable::window() {
  for(size_t i=0; i<nFrames;i++) {
    frames[i].window();
  }
}

inline void wtTable::windowFrame(size_t index) {
  frames[index].window();
}

inline void wtTable::removeDCOffset() {
  for(size_t i=0; i<nFrames;i++) {
    frames[i].removeDCOffset();
  }
}

inline void wtTable::calcFFT() {
  for(size_t i=0; i<nFrames;i++) {
    frames[i].calcFFT();
  }
}

inline void wtTable::removeFrameDCOffset(size_t index) {
  frames[index].removeDCOffset();
}

inline void wtTable::addFrame(size_t index) {
  if (nFrames<NF) {
    if ((nFrames>1) && (index<nFrames-1)) {
      for (size_t i=nFrames-1; i>=index+1; i--) {
        copyFrame(i,i+1);
        frames[i+1].used=frames[i].used;
        frames[i+1].morphed=frames[i].morphed;
      }
    }
    frames[index+1].reset();
    frames[index+1].used=true;
    frames[index+1].morphed=false;
    nFrames++;
  }
}

inline void wtTable::removeFrame(size_t index) {
  if ((nFrames>0) && (index<nFrames)) {
    for (size_t i=index; i<nFrames-1; i++) { copyFrame(i+1,i);}
    nFrames--;
  }
}

void wtTable::morphFrames() {
  deleteMorphing();
  if (nFrames>1) {
    size_t fs = nFrames;
    size_t fCount = (NF-fs)/(fs-1);

    for (size_t i=fs-1; i>0; i--) {
      frames[i].morphed = true;
      frames[i].used = false;
      copyFrame(i, i*(fCount+1));
      frames[i*(fCount+1)].morphed = false;
      frames[i*(fCount+1)].used = true;
    }

    for (size_t i=0; i<fs-1; i++) {
      for (size_t j=1; j<fCount+1; j++) {
        size_t index = i*(fCount+1) + j;
        for(size_t k=0; k<FS; k++) {
          frames[index].sample[k]=rescale(j,0,fCount+1,frames[i*(fCount+1)].sample[k],frames[(i+1)*(fCount+1)].sample[k]);
        }
        frames[index].morphed=true;
        frames[index].used=true;
        nFrames++;
      }
    }
  }
}

void wtTable::morphSpectrum() {
  deleteMorphing();
  if (nFrames>1) {
    size_t fs = nFrames;
    size_t fCount = (NF-fs)/(fs-1);

    frames[0].calcFFT();

    for (size_t i=fs-1; i>0; i--) {
      frames[i].calcFFT();
      frames[i].morphed = true;
      frames[i].used = false;
      copyFrame(i, i*(fCount+1));
      frames[i*(fCount+1)].morphed = false;
      frames[i*(fCount+1)].used = true;
    }

    for (size_t i=0; i<fs-1; i++) {
      for (size_t j=1; j<fCount+1; j++) {
        size_t index = i*(fCount+1) + j;
        for(size_t k=0; k<FS2; k++) {
          frames[index].magnitude[k]=rescale(j,0,fCount+1,frames[i*(fCount+1)].magnitude[k],frames[(i+1)*(fCount+1)].magnitude[k]);
          frames[index].phase[k]=rescale(j,0,fCount+1,frames[i*(fCount+1)].phase[k],frames[(i+1)*(fCount+1)].phase[k]);
        }
        frames[index].calcIFFT();
        frames[index].morphed=true;
        frames[index].used=true;
        nFrames++;
      }
    }
  }
}

void wtTable::morphSpectrumConstantPhase() {
  deleteMorphing();
  if (nFrames>1) {
    size_t fs = nFrames;
    size_t fCount = (NF-fs)/(fs-1);

    frames[0].calcFFT();

    for (size_t i=fs-1; i>0; i--) {
      frames[i].calcFFT();
      for(size_t k=0; k<FS2; k++) {
        frames[i].phase[k]=frames[0].phase[k];
      }
      frames[i].calcIFFT();
      frames[i].morphed = true;
      frames[i].used = false;
      copyFrame(i, i*(fCount+1));
      frames[i*(fCount+1)].morphed = false;
      frames[i*(fCount+1)].used = true;
    }

    for (size_t i=0; i<fs-1; i++) {
      for (size_t j=1; j<fCount+1; j++) {
        size_t index = i*(fCount+1) + j;
        for(size_t k=0; k<FS2; k++) {
          frames[index].magnitude[k]=rescale(j,0,fCount+1,frames[i*(fCount+1)].magnitude[k],frames[(i+1)*(fCount+1)].magnitude[k]);
          frames[index].phase[k]=rescale(j,0,fCount+1,frames[i*(fCount+1)].phase[k],frames[(i+1)*(fCount+1)].phase[k]);
        }
        frames[index].calcIFFT();
        frames[index].morphed=true;
        frames[index].used=true;
        nFrames++;
      }
    }
  }
}

inline void wtTable::reset() {
  for(auto& frame : frames) { frame.reset();}
  nFrames=0;
}

inline void wtTable::init() {
  reset();
  nFrames=NF;
}

void wtTable::deleteMorphing() {
  size_t cm=0;
  size_t cu=0;
  for(size_t i=0; i<nFrames;i++) {
    if (frames[i].morphed) {
      frames[i].used = false;
      cm++;
    }
    else {
      if (i!=cu) {
        copyFrame(i,cu);
        frames[cu].used=true;
        frames[cu].morphed=false;
      }
      cu++;
    }
  }
  nFrames-=cm;
}

template <int OVERSAMPLE, int QUALITY, typename T>
struct wtOscillator {
  wtTable *table;
	bool soft = false;
	bool syncEnabled = false;
	int channels = 0;
  T maxMorph = 2.e-2f;
  T minMorph = 0.f;
  T morph;
  size_t playedIndex = 0;
  size_t targetIndex = 0;

	T lastSyncValue = 0.f;
  T lastOutValue = 0.f;
	T phase = 0.f;
	T freq;
	T syncDirection = 1.f;

	dsp::MinBlepGenerator<QUALITY, OVERSAMPLE, T> minBLEP;

	T outValue = 0.f;

	void setPitch(T pitch) {
		freq = dsp::FREQ_C4 * dsp::approxExp2_taylor5(pitch + 30) / 1073741824;
	}

	void process(float deltaTime, T syncValue, size_t index) {
		T deltaPhase = simd::clamp(freq * deltaTime, 1e-6f, 0.35f);
		if (soft) {
			deltaPhase *= syncDirection;
		}
		else {
			syncDirection = 1.f;
		}
		phase += deltaPhase;
		phase -= simd::floor(phase);


		if (syncEnabled) {
			T deltaSync = syncValue - lastSyncValue;
			T syncCrossing = -lastSyncValue / deltaSync;
			lastSyncValue = syncValue;
			T sync = (0.f < syncCrossing) & (syncCrossing <= 1.f) & (syncValue >= 0.f);
			int syncMask = simd::movemask(sync);
			if (syncMask) {
				if (soft) {
					syncDirection = simd::ifelse(sync, -syncDirection, syncDirection);
				}
				else {
					T newPhase = simd::ifelse(sync, (1.f - syncCrossing) * deltaPhase, phase);
					for (int i = 0; i < channels; i++) {
						if (syncMask & (1 << i)) {
							T mask = simd::movemaskInverse<T>(1 << i);
							float p = syncCrossing[i] - 1.f;
							T x;
							x = mask & (out(deltaTime, newPhase, index) - out(deltaTime, phase, index));
							minBLEP.insertDiscontinuity(p, x);
						}
					}
					phase = newPhase;
				}
			}
		}

		outValue = out(deltaTime, phase, index);

    T deltaOut = outValue - lastOutValue;
    T outCrossing = -lastOutValue / deltaOut;
    lastOutValue = outValue;
    int outMask =  simd::movemask((0.f < outCrossing) & (outCrossing <= 1.f) & (outValue >= 0.f));
    if (outMask) {
      for (int i = 0; i < channels; i++) {
        if (outMask & (1 << i)) {
          T mask = simd::movemaskInverse<T>(1 << i);
          float p = outCrossing[i] - 1.f;
          T x;
          x = mask & (out(deltaTime, phase+deltaPhase-simd::floor(phase+deltaPhase), index) - out(deltaTime, phase, index));
          minBLEP.insertDiscontinuity(p, x);
        }
      }
    }

		outValue += minBLEP.process();
    outValue = clamp(outValue,-10.0f,10.0f);
	}

  T interpolate(const float* p, T x) {
  	T xi = floor(x);
  	T xf = x - xi;
    T pI = {p[(int)xi[0]],p[(int)xi[1]],p[(int)xi[2]],p[(int)xi[3]]};
    T pII = {p[(int)xi[0]+1],p[(int)xi[1]+1],p[(int)xi[2]+1],p[(int)xi[3]+1]};
  	return crossfade(pI, pII, xf);
  }

	T out(float deltaTime, T phase, size_t index) {
		T v;
    if (playedIndex != index) {
      morph = maxMorph;
      targetIndex = index;
    }

    morph-={deltaTime,deltaTime,deltaTime,deltaTime};

    if (morph[0]<=0) {
      playedIndex=targetIndex;
    }

    if (playedIndex==targetIndex) {
      v = interpolate(table->frames[playedIndex].sample.data(), phase * 2047.f);
    }
    else {
      T pVal = interpolate(table->frames[playedIndex].sample.data(), phase * 2047.f);
      T tVal = interpolate(table->frames[targetIndex].sample.data(), phase * 2047.f);
      v = rescale(morph,minMorph,maxMorph,pVal,tVal);
    }

		return v;
	}

	T out() {
		return outValue;
	}

};
