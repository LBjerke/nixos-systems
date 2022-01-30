/*
  ZynAddSubFX - a software synthesizer

  Fl_Osc_Counter.cpp - OSC Powered Counter
  Copyright (C) 2016 Mark McCurry

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/
#include "Fl_Osc_Counter.H"

static void callback_fn_counter(Fl_Widget *w, void *)
{
    ((Fl_Osc_Counter*)w)->cb();
}

Fl_Osc_Counter::Fl_Osc_Counter(int x, int y, int w, int h, const char *label)
    :Fl_Counter(x,y,w,h,label), Fl_Osc_Widget(this), offset(0)
{
    Fl_Counter::callback(callback_fn_counter);
}

void Fl_Osc_Counter::update(void)
{
    oscWrite(ext);
}

void Fl_Osc_Counter::init(const char *path_, char type_, int display_off)
{
    offset = display_off;
    oscRegister(path_);
    ext = path_;
    cb_type = type_;
}

void Fl_Osc_Counter::callback(Fl_Callback *cb, void *p)
{
    cb_data.first = cb;
    cb_data.second = p;
}

void Fl_Osc_Counter::OSC_value(int v)
{
    value(v+offset);
}

void Fl_Osc_Counter::OSC_value(char v)
{
    value(v+offset);
}

void Fl_Osc_Counter::cb(void)
{
    assert(osc);

    if(cb_type == 'c') {
        fprintf(stderr, "invalid `c' from counter %s%s, using `i'\n", loc.c_str(), ext.c_str());
        oscWrite(ext, "i", (int)(value()-offset));
    }
    else
        oscWrite(ext, "i", (int)(value()-offset));

    if(cb_data.first)
        cb_data.first(this, cb_data.second);
}
