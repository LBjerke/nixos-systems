/******************************************************************************
 * Copyright 2017-2019 Valerio Orlandini / Sonus Dept. <sonusdept@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/


#include "rack.hpp"

#include <climits>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <deque>
#include <vector>


using namespace rack;


extern Plugin *pluginInstance;


struct SonusKnob : SvgKnob
{
    SonusKnob()
    {
        box.size = Vec(36, 36);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/knob.svg")));
    }
};

struct SonusSnapKnob : SonusKnob
{
    SonusSnapKnob()
    {
        snap = true;
    }
};

struct SonusBigKnob : SvgKnob
{
    SonusBigKnob()
    {
        box.size = Vec(54, 54);
        minAngle = -0.75 * M_PI;
        maxAngle = 0.75 * M_PI;
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/bigknob.svg")));
    }
};

struct SonusBigSnapKnob : SonusBigKnob
{
    SonusBigSnapKnob()
    {
        snap = true;
    }
};

struct SonusScrew : SvgKnob
{
    SonusScrew()
    {
        minAngle = -0.99 * M_PI;
        maxAngle = 0.99 * M_PI;
        box.size = Vec(15, 15);
        setSvg(APP->window->loadSvg(asset::plugin(pluginInstance, "res/screw.svg")));
    }
};

struct SonusLedButton : SvgSwitch
{
    SonusLedButton()
    {
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ledbutton_off.svg")));
        addFrame(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ledbutton_on.svg")));
    }
};


extern Model *modelAddiction;
extern Model *modelBitter;
extern Model *modelBymidside;
extern Model *modelCampione;
extern Model *modelChainsaw;
extern Model *modelCtrl;
extern Model *modelDeathcrush;
extern Model *modelFraction;
extern Model *modelHarmony;
extern Model *modelLadrone;
extern Model *modelLuppolo;
extern Model *modelLuppolo3;
extern Model *modelMicromacro;
extern Model *modelMrcheb;
extern Model *modelMultimulti;
extern Model *modelNeurosc;
extern Model *modelOktagon;
extern Model *modelOsculum;
extern Model *modelParamath;
extern Model *modelPiconoise;
extern Model *modelPith;
extern Model *modelPusher;
extern Model *modelRingo;
extern Model *modelScramblase;
extern Model *modelTropicana;
extern Model *modelTwoff;
extern Model *modelYabp;
