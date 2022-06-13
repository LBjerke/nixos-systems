/*
  ZynAddSubFX - a software synthesizer

  Envelope.cpp - Envelope implementation
  Copyright (C) 2002-2005 Nasca Octavian Paul
  Author: Nasca Octavian Paul

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/

#include <cmath>
#include "Envelope.h"
#include "../Params/EnvelopeParams.h"

namespace zyncarla {

Envelope::Envelope(EnvelopeParams &pars, float basefreq, float bufferdt,
        WatchManager *m, const char *watch_prefix)
    :watchOut(m, watch_prefix, "out")
{
    envpoints = pars.Penvpoints;
    if(envpoints > MAX_ENVELOPE_POINTS)
        envpoints = MAX_ENVELOPE_POINTS;
    envsustain     = (pars.Penvsustain == 0) ? -1 : pars.Penvsustain;
    forcedrelease   = pars.Pforcedrelease;
    envstretch     = powf(440.0f / basefreq, pars.Penvstretch / 64.0f);
    linearenvelope = pars.Plinearenvelope;

    if(!pars.Pfreemode)
        pars.converttofree();

    int mode = pars.Envmode;

    //for amplitude envelopes
    if((mode == 1) && !linearenvelope)
        mode = 2;                              //change to log envelope
    if((mode == 2) && linearenvelope)
        mode = 1;                              //change to linear

    for(int i = 0; i < MAX_ENVELOPE_POINTS; ++i) {
        const float tmp = pars.getdt(i) / 1000.0f * envstretch;
        if(tmp > bufferdt)
            envdt[i] = bufferdt / tmp;
        else
            envdt[i] = 2.0f;  //any value larger than 1

        switch(mode) {
            case 2:
                envval[i] = (1.0f - pars.Penvval[i] / 127.0f) * -40;
                break;
            case 3:
                envval[i] =
                    (powf(2, 6.0f
                          * fabs(pars.Penvval[i]
                                 - 64.0f) / 64.0f) - 1.0f) * 100.0f;
                if(pars.Penvval[i] < 64)
                    envval[i] = -envval[i];
                break;
            case 4:
                envval[i] = (pars.Penvval[i] - 64.0f) / 64.0f * 6.0f; //6 octaves (filtru)
                break;
            case 5:
                envval[i] = (pars.Penvval[i] - 64.0f) / 64.0f * 10;
                break;
            default:
                envval[i] = pars.Penvval[i] / 127.0f;
        }
    }

    envdt[0] = 1.0f;

    currentpoint = 1; //the envelope starts from 1
    keyreleased  = false;
    t = 0.0f;
    envfinish = false;
    inct      = envdt[1];
    envoutval = 0.0f;
}

Envelope::~Envelope()
{}


/*
 * Release the key (note envelope)
 */
void Envelope::releasekey()
{
    if(keyreleased)
        return;
    keyreleased = true;
    if(forcedrelease)
        t = 0.0f;
}

void Envelope::forceFinish(void)
{
    envfinish = true;
}

/*
 * Envelope Output
 */
float Envelope::envout(bool doWatch)
{
    float out;

    if(envfinish) { //if the envelope is finished
        envoutval = envval[envpoints - 1];
        if(doWatch) {
            float pos[2] = {(float)envpoints - 1, envoutval};
            watchOut(pos, 2);
        }
        return envoutval;
    }
    if((currentpoint == envsustain + 1) && !keyreleased) { //if it is sustaining now
        envoutval = envval[envsustain];
        if(doWatch) {
            float pos[2] = {(float)envsustain, envoutval};
            watchOut(pos, 2);
        }
        return envoutval;
    }

    if(keyreleased && forcedrelease) { //do the forced release
        int tmp = (envsustain < 0) ? (envpoints - 1) : (envsustain + 1); //if there is no sustain point, use the last point for release

        if(envdt[tmp] < 0.00000001f)
            out = envval[tmp];
        else
            out = envoutval + (envval[tmp] - envoutval) * t;
        t += envdt[tmp] * envstretch;

        if(t >= 1.0f) {
            currentpoint = envsustain + 2;
            forcedrelease = 0;
            t    = 0.0f;
            inct = envdt[currentpoint];
            if((currentpoint >= envpoints) || (envsustain < 0))
                envfinish = true;
        }

        if(doWatch) {
            float pos[2] = {(float)tmp + t, envoutval};
            watchOut(pos, 2);
        }

        return out;
    }
    if(inct >= 1.0f)
        out = envval[currentpoint];
    else
        out = envval[currentpoint - 1]
              + (envval[currentpoint] - envval[currentpoint - 1]) * t;

    t += inct;
    if(t >= 1.0f) {
        if(currentpoint >= envpoints - 1)
            envfinish = true;
        else
            currentpoint++;
        t    = 0.0f;
        inct = envdt[currentpoint];
    }

    envoutval = out;

    if(doWatch) {
        float pos[2] = {(float)currentpoint + t, envoutval};
        watchOut(pos, 2);
    }
    return out;
}

/*
 * Envelope Output (dB)
 */
float Envelope::envout_dB()
{
    float out;
    if(linearenvelope)
        return envout(true);

    if((currentpoint == 1) && (!keyreleased || !forcedrelease)) { //first point is always lineary interpolated
        float v1 = EnvelopeParams::env_dB2rap(envval[0]);
        float v2 = EnvelopeParams::env_dB2rap(envval[1]);
        out = v1 + (v2 - v1) * t;

        t += inct;
        if(t >= 1.0f) {
            t    = 0.0f;
            inct = envdt[2];
            currentpoint++;
            out = v2;
        }

        if(out > 0.001f)
            envoutval = EnvelopeParams::env_rap2dB(out);
        else
            envoutval = MIN_ENVELOPE_DB;
    } else
        out = EnvelopeParams::env_dB2rap(envout(false));

    float pos[2] = {(float)currentpoint + t, out};
    watchOut(pos, 2);

    return out;
}

bool Envelope::finished() const
{
    return envfinish;
}

}
