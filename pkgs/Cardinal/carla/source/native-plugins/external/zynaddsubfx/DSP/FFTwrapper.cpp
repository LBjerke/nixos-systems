/*
  ZynAddSubFX - a software synthesizer

  FFTwrapper.c  -  A wrapper for Fast Fourier Transforms
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/

#include <cmath>
#include <cassert>
#include <cstring>
#include <pthread.h>
#include "FFTwrapper.h"

namespace zyncarla {

static pthread_mutex_t *mutex = NULL;

FFTwrapper::FFTwrapper(int fftsize_)
{
    //first one will spawn the mutex (yeah this may be a race itself)
    if(!mutex) {
        mutex = new pthread_mutex_t;
        pthread_mutex_init(mutex, NULL);
    }


    fftsize  = fftsize_;
    time     = new fftw_real[fftsize];
    fft      = new fftw_complex[fftsize + 1];
    pthread_mutex_lock(mutex);
    planfftw = fftw_plan_dft_r2c_1d(fftsize,
                                    time,
                                    fft,
                                    FFTW_ESTIMATE);
    planfftw_inv = fftw_plan_dft_c2r_1d(fftsize,
                                        fft,
                                        time,
                                        FFTW_ESTIMATE);
    pthread_mutex_unlock(mutex);
}

FFTwrapper::~FFTwrapper()
{
    pthread_mutex_lock(mutex);
    fftw_destroy_plan(planfftw);
    fftw_destroy_plan(planfftw_inv);
    pthread_mutex_unlock(mutex);

    delete [] time;
    delete [] fft;
}

void FFTwrapper::smps2freqs(const float *smps, fft_t *freqs)
{
    //Load data
    for(int i = 0; i < fftsize; ++i)
        time[i] = static_cast<double>(smps[i]);

    //DFT
    fftw_execute(planfftw);

    //Grab data
    memcpy((void *)freqs, (const void *)fft, fftsize * sizeof(double));
}

void FFTwrapper::freqs2smps(const fft_t *freqs, float *smps)
{
    //Load data
    memcpy((void *)fft, (const void *)freqs, fftsize * sizeof(double));

    //clear unused freq channel
    fft[fftsize / 2][0] = 0.0f;
    fft[fftsize / 2][1] = 0.0f;

    //IDFT
    fftw_execute(planfftw_inv);

    //Grab data
    for(int i = 0; i < fftsize; ++i)
        smps[i] = static_cast<float>(time[i]);
}

void FFT_cleanup()
{
    fftw_cleanup();
    pthread_mutex_destroy(mutex);
    delete mutex;
    mutex = NULL;
}

}
