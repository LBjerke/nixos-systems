/*
  ZynAddSubFX - a software synthesizer

  Recorder.cpp - Records sound to a file
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/

#include <sys/stat.h>
#include "Recorder.h"
#include "WavFile.h"
#include "../globals.h"
#include "../Nio/Nio.h"

namespace zyncarla {

Recorder::Recorder(const SYNTH_T &synth_)
    :status(0), notetrigger(0),synth(synth_)
{}

Recorder::~Recorder()
{
    if(recording() == 1)
        stop();
}

int Recorder::preparefile(std::string filename_, int overwrite)
{
    if(!overwrite) {
        struct stat fileinfo;
        int statr;
        statr = stat(filename_.c_str(), &fileinfo);
        if(statr == 0)   //file exists
            return 1;
    }

    Nio::waveNew(new WavFile(filename_, synth.samplerate, 2));

    status = 1; //ready

    return 0;
}

void Recorder::start()
{
    notetrigger = 0;
    status      = 2; //recording
}

void Recorder::stop()
{
    Nio::waveStop();
    Nio::waveStart();
    status = 0;
}

void Recorder::pause()
{
    status = 0;
    Nio::waveStop();
}

int Recorder::recording()
{
    if((status == 2) && (notetrigger != 0))
        return 1;
    else
        return 0;
}

void Recorder::triggernow()
{
    if(status == 2) {
        if(notetrigger != 1)
            Nio::waveStart();
        notetrigger = 1;
    }
}

//TODO move recorder inside nio system
}
