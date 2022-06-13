/*
  ZynAddSubFX - a software synthesizer

  OSSaudiooutput.h - Audio output for Open Sound System
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/

#ifndef OSS_ENGINE_H
#define OSS_ENGINE_H

#include <sys/time.h>
#include "../globals.h"
#include "AudioOut.h"
#include "MidiIn.h"

namespace zyncarla {

struct OssMidiParse {
        unsigned char *temp_cmd;
        unsigned char temp_0[4];
        unsigned char temp_1[4];
        unsigned char state;
#define OSSMIDI_ST_UNKNOWN   0          /* scan for command */
#define OSSMIDI_ST_1PARAM    1
#define OSSMIDI_ST_2PARAM_1  2
#define OSSMIDI_ST_2PARAM_2  3
#define OSSMIDI_ST_SYSEX_0   4
#define OSSMIDI_ST_SYSEX_1   5
#define OSSMIDI_ST_SYSEX_2   6
};

class OssEngine:public AudioOut, MidiIn
{
    public:
        OssEngine(const SYNTH_T &synth, const class oss_devs_t& oss_devs);
        ~OssEngine();

        bool Start();
        void Stop();

        void setAudioEn(bool nval);
        bool getAudioEn() const;

        void setMidiEn(bool nval);
        bool getMidiEn() const;

    protected:
        void *audioThreadCb();
        static void *_audioThreadCb(void *arg);

        void *midiThreadCb();
        static void *_midiThreadCb(void *arg);

    private:
        pthread_t *audioThread;
        pthread_t *midiThread;

        //Audio
        bool openAudio();
        void stopAudio();

        struct audio {
            int handle;
            int buffersize;
            union {
                /* Samples to be sent to soundcard */
                short int *ps16;
                int *ps32;
            } smps;

            /* peak values used for compressor */
            float peaks[1];

            bool en;
            bool is32bit;
        } audio;
        
        const char* linux_oss_wave_out_dev;

        //Midi
        bool openMidi();
        void stopMidi();

        struct midi {
            struct OssMidiParse state;
            int  handle;
            bool en;
            bool run;
        } midi;
        
        const char* linux_oss_seq_in_dev;
};

}

#endif
