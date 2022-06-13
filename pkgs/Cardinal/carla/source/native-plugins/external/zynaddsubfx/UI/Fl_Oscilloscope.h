/*
  ZynAddSubFX - a software synthesizer

  Fl_Oscilloscope.h - OSC Waveform View
  Copyright (C) 2016 Mark McCurry

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/
#pragma once

#include "Fl_Osc_Pane.H"
#include <FL/Fl_Box.H>
#include <FL/fl_draw.H>
#include "Fl_Osc_Widget.H"
#include "Fl_Osc_Pane.H"
#include "Fl_Osc_Interface.h"
#include "common.H"
#include <cassert>
#include <cstdio>
#include "../globals.h"

class Fl_Osc_Group;
//Consider merging in Fl_OscilSpectrum
class Fl_Oscilloscope : public Fl_Box, public Fl_Osc_Widget
{
    public:
        Fl_Oscilloscope(int x,int y, int w, int h, const char *label=0)
            :Fl_Box(x,y,w,h,label), Fl_Osc_Widget(this), smps(0), oscilsize(0),
            Overlay(NULL)
        {
            phase=64;
            box(FL_FLAT_BOX);
            bkgnd = fl_color_average( FL_BLACK, FL_BACKGROUND_COLOR, 0.5 );
        }

        ~Fl_Oscilloscope(void)
        {
            delete[] smps;
        }

        void init(bool base_waveform_p)
        {
            ext = (base_waveform_p ? "base-waveform": "waveform");
            osc->createLink("/oscilsize", this);
            osc->requestValue("/oscilsize");
            assert(osc);
            oscRegister(ext.c_str());
        }

        void update(void)
        {
            oscWrite(ext);
        }

        virtual void OSC_value(int smp)
        {
            if(smp == oscilsize)
                return;
            oscilsize = smp;
            delete []smps;
            smps = new float[oscilsize];
            memset(smps, 0, oscilsize*sizeof(float));
        }

        virtual void OSC_value(unsigned N, void *data) override
        {
            if(oscilsize == 0)
                OSC_value((int)N/4);

            assert(N==(unsigned)(oscilsize*4));

            memcpy(smps, data, N);

            //normalize
            float max=0;
            for (int i=0;i<oscilsize;i++)
                if(max<fabs(smps[i]))
                    max=fabs(smps[i]);
            if (max<0.00001) max=1.0;
            max *= -1.05;

            for(int i=0; i < oscilsize; ++i)
                smps[i] /= max;

            //Get widget to redraw new data
            redraw();
        }

        void draw(void)
        {
            int ox=x(),oy=y(),lx=w(),ly=h()-1;

            if (damage()!=1) {
                fl_color(bkgnd);
                fl_rectf(ox,oy,lx,ly);
            }

            //draw
            fl_line_style(FL_DASH);
            if (this->active_r()) fl_color(this->parent()->labelcolor());
            else fl_color(this->parent()->color());

            int GX=16;if (lx<GX*3) GX=-1;
            for (int i=1;i<GX;i++){
                int tmp=(int)(lx/(float)GX*i);
                fl_line(ox+tmp,oy+2,ox+tmp,oy+ly-2);
            }

            int GY=8; if (ly<GY*3) GY=-1;
            for (int i=1;i<GY;i++){
                int tmp=(int)(ly/(float)GY*i);
                fl_line(ox+2,oy+tmp,ox+lx-2,oy+tmp);
            }

            //draw the function
            fl_line_style(0,1);
            fl_line(ox+2,oy+ly/2,ox+lx-2,oy+ly/2);
            if (this->active_r()) fl_color(this->parent()->selection_color());
            else fl_color(this->parent()->labelcolor());

            fl_color( fl_color_add_alpha( fl_color(), 127 ) );

            if(smps) {
                int lw=2;
                fl_line_style(FL_SOLID,lw);
                fl_begin_line();
                double ph=((phase-64.0)/128.0*oscilsize+oscilsize);
                for (int i=1;i<lx;i++){
                    int k2=(oscilsize*i/lx)+ph;
                    fl_vertex(i+ox,(smps[k2%oscilsize]+1)*(ly-1)/2+oy+0.5);
                }
                fl_end_line();
            }

            // Erase stray pixels on margin
            fl_color(bkgnd);
            fl_line_style(FL_SOLID,1);
            fl_rect(ox-1,oy-1,lx+2,ly+2);

            fl_line_style(FL_SOLID,0);
            if (Overlay)
                Overlay->redraw();
        }

        //allows UI to manipuate phase of displayed waveform
        int phase;

    private:
        Fl_Osc_Pane *fetch_osc_pane(Fl_Widget *w)
        {
            if(!w)
                return NULL;

            Fl_Osc_Pane *pane = dynamic_cast<Fl_Osc_Pane*>(w->parent());
            if(pane)
                return pane;
            return fetch_osc_pane(w->parent());
        }

        float *smps;
        int oscilsize;
        Fl_Color bkgnd;
    public:
        Fl_Box *Overlay;
};
