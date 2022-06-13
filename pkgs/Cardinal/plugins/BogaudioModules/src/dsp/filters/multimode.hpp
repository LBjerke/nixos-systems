#pragma once

#include <complex>

#include "filters/filter.hpp"

#ifdef RACK_SIMD
#include "simd/Vector.hpp"
using rack::simd::float_4;
#endif

namespace bogaudio {
namespace dsp {

#ifdef RACK_SIMD
	struct Biquad4 {
		float_4 _a0;
		float_4 _a1;
		float_4 _a2;
		float_4 _b1;
		float_4 _b2;
		float_4 _x[3] {};
		float_4 _y[3] {};
		bool _disable = false;
		int _outputIndex = 3;

		void setParams(int i, float a0, float a1, float a2, float b0, float b1, float b2);
		void reset();
		void setN(int n, bool minDelay = false);
		inline void disable(bool disable) { _disable = disable; }
		float next(float sample);
	};
#endif

template<typename T, int N>
struct BiquadBank : ResetableFilter {
#ifdef RACK_SIMD
	Biquad4 _biquads[N / 4];
#else
	BiquadFilter<T> _biquads[N];
	int _n = N;
#endif

	void setParams(int i, T a0, T a1, T a2, T b0, T b1, T b2);
	void reset() override;
	void setN(int n, bool minDelay = false);
	float next(float sample) override;
};

struct MultimodeTypes {
	typedef float T;
	typedef std::complex<T> TC;

	enum Type {
		UNKNOWN_TYPE,
		BUTTERWORTH_TYPE,
		CHEBYSHEV_TYPE
	};

	enum Mode {
		UNKNOWN_MODE,
		LOWPASS_MODE,
		HIGHPASS_MODE,
		BANDPASS_MODE,
		BANDREJECT_MODE
	};

	enum BandwidthMode {
		UNKNOWN_BANDWIDTH_MODE,
		LINEAR_BANDWIDTH_MODE,
		PITCH_BANDWIDTH_MODE
	};

	enum DelayMode {
		UNKNOWN_DELAY_MODE,
		FIXED_DELAY_MODE,
		MINIMUM_DELAY_MODE
	};

	static constexpr int minPoles = 1;
	static constexpr int maxPoles = 16;
	static constexpr int modPoles = 1;
	static constexpr float minFrequency = 3.0f; // FIXME: this can go down to at least 1.0f if T is double.
	static constexpr float maxFrequency = 21000.0f;
	static constexpr float minQbw = 0.0f;
	static constexpr float maxQbw = 1.0f;
	static constexpr float minBWLinear = 10.0f;
	static constexpr float maxBWLinear = 5000.0f;
	static constexpr float minBWPitch = 1.0f / (1.0f * 12.0f * 100.0f / 25.0f);
	static constexpr float maxBWPitch = 2.0f;
};

template<int N>
struct MultimodeDesigner : MultimodeTypes {
	struct Pole {
		TC p;
		T x = 0.0;
		T y = 0.0;
		TC pc;
		TC p2;
		TC i2p;
		TC i2pc;
		T r = 0.0;

		Pole() {}
		Pole(T re, T im, T x, T y) : p(TC(re, im)), x(x), y(y) {
			pc = std::conj(p);
			p2 = p * p;
			i2p = (T)1.0 / ((T)2.0 * p);
			i2pc = (T)1.0 / ((T)2.0 * pc);
			r = std::abs(p);
		}
	};

	float _sampleRate = 44100.0f;
	float _half2PiST = 0.0f;
	Type _type = UNKNOWN_TYPE;
	Mode _mode = UNKNOWN_MODE;
	int _nPoles = 0;
	float _frequency = -1.0f;
	float _qbw = -1.0f;
	BandwidthMode _bandwidthMode = UNKNOWN_BANDWIDTH_MODE;
	DelayMode _delayMode = UNKNOWN_DELAY_MODE;
	Pole _poles[maxPoles / 2];
	int _nBiquads = 0;

	float effectiveMinimumFrequency();
	void setParams(
		BiquadBank<T, N>& biquads,
		float& outGain,
		float sampleRate,
		Type type,
		int poles,
		Mode mode,
		float frequency,
		float qbw,
		BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
		DelayMode dm = FIXED_DELAY_MODE
	);
};

struct MultimodeFilter : MultimodeTypes, ResetableFilter {
	virtual void setParams(
		float sampleRate,
		Type type,
		int poles,
		Mode mode,
		float frequency,
		float qbw,
		BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
		DelayMode dm = FIXED_DELAY_MODE
	) = 0;
};

template<int N>
struct MultimodeBase : MultimodeFilter {
	MultimodeDesigner<N> _designer;
	BiquadBank<T, N> _biquads;
	float _outGain = 1.0f;

	inline float effectiveMinimumFrequency() { return _designer.effectiveMinimumFrequency(); }
	void setParams(
		float sampleRate,
		Type type,
		int poles,
		Mode mode,
		float frequency,
		float qbw,
		BandwidthMode bwm = PITCH_BANDWIDTH_MODE,
		DelayMode dm = FIXED_DELAY_MODE
	) override;
	float next(float sample) override;
	void reset() override;
};

typedef MultimodeBase<16> MultimodeFilter16;
typedef MultimodeBase<8> MultimodeFilter8;
typedef MultimodeBase<4> MultimodeFilter4;

struct FourPoleButtworthLowpassFilter {
	MultimodeFilter4 _filter;

	inline void setParams(
		float sampleRate,
		float frequency,
		float q
	) {
		_filter.setParams(
			sampleRate,
			MultimodeFilter::BUTTERWORTH_TYPE,
			4,
			MultimodeFilter::LOWPASS_MODE,
			frequency,
			q
		);
	}
	inline float next(float sample) { return _filter.next(sample); }
	inline void reset() { _filter.reset(); }
};

struct FourPoleButtworthHighpassFilter {
	MultimodeFilter4 _filter;

	inline void setParams(
		float sampleRate,
		float frequency,
		float q
	) {
		_filter.setParams(
			sampleRate,
			MultimodeFilter::BUTTERWORTH_TYPE,
			4,
			MultimodeFilter::HIGHPASS_MODE,
			frequency,
			q
		);
	}
	inline float next(float sample) { return _filter.next(sample); }
	inline void reset() { _filter.reset(); }
};

struct TwoPoleButtworthBandpassFilter {
	MultimodeFilter4 _filter;

	inline void setParams(
		float sampleRate,
		float frequency,
		float bw,
		MultimodeFilter::BandwidthMode bwm = MultimodeFilter::PITCH_BANDWIDTH_MODE
	) {
		_filter.setParams(
			sampleRate,
			MultimodeFilter::BUTTERWORTH_TYPE,
			2,
			MultimodeFilter::BANDPASS_MODE,
			frequency,
			bw,
			bwm
		);
	}
	inline float next(float sample) { return _filter.next(sample); }
	inline void reset() { _filter.reset(); }
};

struct FourPoleButtworthBandpassFilter {
	MultimodeFilter4 _filter;

	inline void setParams(
		float sampleRate,
		float frequency,
		float bw,
		MultimodeFilter::BandwidthMode bwm = MultimodeFilter::PITCH_BANDWIDTH_MODE
	) {
		_filter.setParams(
			sampleRate,
			MultimodeFilter::BUTTERWORTH_TYPE,
			4,
			MultimodeFilter::BANDPASS_MODE,
			frequency,
			bw,
			bwm
		);
	}
	inline float next(float sample) { return _filter.next(sample); }
	inline void reset() { _filter.reset(); }
};

} // namespace dsp
} // namespace bogaudio
