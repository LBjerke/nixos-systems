/*
  ZynAddSubFX - a software synthesizer

  Fl_PADnoteOvertonePosition.h - PADnote Specific Spectrum View
  Copyright (C) 2016 Mark McCurry

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/
#include <cstring>
#include <cassert>
#include <FL/Fl.H>
#include <FL/Fl_Box.H>
#include "../globals.h"
#include "Fl_Osc_Widget.H"
#include "Fl_Osc_Interface.h"

class PADnoteOvertonePosition: public Fl_Box, public Fl_Osc_Widget
{
    public:
        PADnoteOvertonePosition(int x,int y, int w, int h, const char *label=0)
            :Fl_Box(x,y,w,h,label), nsamples(0),
            spc(0),
            nhr(0),
            spectrum(new float[w]),
            mode(0), osc(NULL)
        {
            memset(spectrum, 0, w*sizeof(float));
        }

        ~PADnoteOvertonePosition(void)
        {
            osc->removeLink("/oscilsize",
                    (Fl_Osc_Widget*) this);
            osc->removeLink(base_path + "oscilgen/spectrum",
                    (Fl_Osc_Widget*) this);
            osc->removeLink(base_path + "nhr",
                    (Fl_Osc_Widget*) this);
            osc->removeLink(base_path + "Pmode",
                    (Fl_Osc_Widget*) this);
            delete [] spc;
            delete [] nhr;
            delete [] spectrum;
        }

        void init(void)
        {
            Fl_Osc_Pane *og  = fetch_osc_pane(this);
            assert(og);

            base_path = og->base;
            osc = og->osc;
            assert(osc);
            
            osc->createLink("/oscilsize", (Fl_Osc_Widget*) this);
            osc->requestValue("/oscilsize");

            osc->createLink(base_path + "nhr",
                    (Fl_Osc_Widget*) this);
            osc->createLink(base_path + "oscilgen/spectrum",
                    (Fl_Osc_Widget*) this);
            osc->createLink(base_path + "Pmode",
                    (Fl_Osc_Widget*) this);

            update();
        }

        void update(void)
        {
            osc->requestValue(base_path + "nhr");
            osc->requestValue(base_path + "oscilgen/spectrum");
            osc->requestValue(base_path + "Pmode");
        }

        virtual void OSC_value(unsigned N, void *data, const char *name)
            override
        {
            if(N/4 != nsamples) {
                nsamples = N/4;
                delete [] spc;
                delete [] nhr;
                spc = new float[nsamples];
                nhr = new float[nsamples];
                memset(spc, 0, nsamples*sizeof(float));
                memset(nhr, 0, nsamples*sizeof(float));
            }
            assert(N==(4*nsamples));
            float *d = (float*)data;
            if(!strcmp(name, "spectrum"))
                updateSpectrum(d);
            else if(!strcmp(name, "nhr"))
                updateHarmonicPos(d);
            else
                assert(false);
        }
        virtual void OSC_value(int x, const char *name) override
        {
            if(!strcmp(name, "Pmode")) {
                mode = x;
                regenerateOvertones();
            } else if(!strcmp(name, "oscilsize")) {
                if(x/2 != (int)nsamples) {
                    nsamples = x/2;
                    delete [] spc;
                    delete [] nhr;
                    spc = new float[nsamples];
                    nhr = new float[nsamples];
                    memset(spc, 0, nsamples*sizeof(float));
                    memset(nhr, 0, nsamples*sizeof(float));
                }
            }
        }

    private:
        void updateSpectrum(float *data)
        {
            //normalize
            float max=0;
            for (unsigned i=0; i<nsamples; i++){
                const float x=fabs(data[i]);
                if (max<x) max=x;
            }
            if (max<0.000001) max=1.0;
            max=max*1.05;

            for(unsigned i=0; i<nsamples; ++i)
                spc[i] = data[i]/max;
            regenerateOvertones();
        }

        void updateHarmonicPos(float *data)
        {
            memcpy(nhr, data, nsamples*sizeof(float));
            regenerateOvertones();
        }

        void regenerateOvertones(void)
        {
            const int ox=x(),oy=y(),lx=w(),ly=h();
            (void)ox;(void)oy;(void)lx;(void)ly;
            const int maxharmonic=64;

            memset(spectrum, 0, lx*sizeof(float));

            for (unsigned i=1;i<nsamples;i++){
                int kx=(int)(lx/(float)maxharmonic*nhr[i]);
                if ((kx<0)||(kx>=lx)) continue;

                spectrum[kx]=spc[i-1]+1e-9;
            }


            if(mode==2) {
                int old=0;
                for (int i=1;i<lx;i++){
                    if ((spectrum[i]>1e-10)||(i==(lx-1))){
                        const int delta=i-old;
                        const float val1=spectrum[old];
                        const float val2=spectrum[i];

                        const float idelta=1.0/delta;
                        for (int j=0;j<delta;j++) {
                            const float x=idelta*j;
                            spectrum[old+j]=val1*(1.0-x)+val2*x;
                        }
                        old=i;
                    }

                }
            }
            redraw();
        }

        void draw(void)
        {
            const int ox=x(),oy=y(),lx=w(),ly=h();
            const int maxharmonic=64;
            const int maxdb=60;

            if (!visible())
                return;
            
            if (damage()!=1){
                fl_color(fl_color_average(FL_BLACK,
                            FL_BACKGROUND_COLOR, 0.5 ));
                fl_rectf(ox,oy,lx,ly);
            }


            for (int i=1;i<maxharmonic;i++){
                fl_color(100,100,100);
                fl_line_style(FL_DOT);
                if (i%5==0) fl_line_style(0);
                if (i%10==0) fl_color(120,120,120);
                int kx=(int)(lx/(float)maxharmonic*i);
                fl_line(ox+kx,oy,ox+kx,oy+ly);
            };
            
            fl_color(180,0,0);
            fl_line_style(0);

            for (int i=0;i<lx;i++){
                float x=spectrum[i];
                if (x>dB2rap(-maxdb)) x=rap2dB(x)/maxdb+1;
                else continue;
                int yy=(int)(x*ly);
                fl_line(ox+i,oy+ly-1-yy,ox+i,oy+ly-1);

            }
        }

    private:
        size_t nsamples;
        float *spc;
        float *nhr;
        float *spectrum;
        char mode;

        std::string base_path;
        Fl_Osc_Interface *osc;
}; 
