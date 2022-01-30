/*
   ZynAddSubFX - a software synthesizer

   OssMultiEngine.h - Multi channel audio output for Open Sound System
   Copyright (C) 2014 Hans Petter Selasky

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/

#ifndef OSS_MULTI_ENGINE_H
#define OSS_MULTI_ENGINE_H

#include <sys/time.h>
#include "../globals.h"
#include "AudioOut.h"

namespace zyncarla {

class OssMultiEngine : public AudioOut
{
    public:
        OssMultiEngine(const SYNTH_T &synth,
                       const class oss_devs_t& oss_devs);
        ~OssMultiEngine();

        bool Start();
        void Stop();

        void setAudioEn(bool nval);
        bool getAudioEn() const;

    protected:
        void *audioThreadCb();
        static void *_audioThreadCb(void *arg);

    private:
        pthread_t audioThread;

        /* Audio */
        bool openAudio();
        void stopAudio();

        int handle;
        int maxbuffersize;
        int buffersize;
        int channels;

        union {
            /* Samples to be sent to soundcard */
            short int *ps16;
            int *ps32;
        } smps;

        /* peak values used for compressor */
        float *peaks;

        bool en;
        bool is32bit;

        const char* linux_oss_wave_out_dev;
};

}

#endif
