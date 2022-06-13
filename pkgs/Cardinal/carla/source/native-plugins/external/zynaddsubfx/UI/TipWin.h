/*
  ZynAddSubFX - a software synthesizer

  TipWin.h - Tooltips
  Copyright (C) 2016 Mark McCurry

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/
#ifndef TIPWIN_H
#include <string>
#include <FL/Fl_Menu_Window.H>
#include <FL/Fl_Tooltip.H>
#define TIPWIN_H
using namespace std;

class TipWin:public Fl_Menu_Window
{
    public:
        TipWin();
        void draw();
        void showValue(float f);
        void setText(const char *c);
        void showText();
        void set_rounding(unsigned int digits = 0);
    private:
        void redraw();
        const char *getStr() const;
        string tip;
        string text;
        bool   textmode;
        char  format[6];
};
#endif
