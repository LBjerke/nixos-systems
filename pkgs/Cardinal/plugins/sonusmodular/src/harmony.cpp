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


#include "sonusmodular.hpp"


struct Harmony : Module
{
    enum ParamIds
    {
        NUM_PARAMS
    };
    enum InputIds
    {
        PITCH,
        NUM_INPUTS
    };
    enum OutputIds
    {
        m2,
        M2,
        m3,
        M3,
        P4,
        TT,
        P5,
        m6,
        M6,
        m7,
        M7,
        P8,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Harmony()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }
    void process(const ProcessArgs &args) override;

    const float semitone = 1.0 / 12.0;
};


void Harmony::process(const ProcessArgs &args)
{
    float pitch = inputs[PITCH].getVoltage();

    for (int s = 0; s < 12; s++)
    {
        outputs[s].setVoltage(pitch + (semitone * (s + 1)));
    }
}

struct HarmonyWidget : ModuleWidget
{
    HarmonyWidget(Harmony *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/harmony.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(78, 67), module, Harmony::PITCH));

        addOutput(createOutput<PJ301MPort>(Vec(20, 132), module, Harmony::m2));
        addOutput(createOutput<PJ301MPort>(Vec(58, 132), module, Harmony::M2));
        addOutput(createOutput<PJ301MPort>(Vec(96, 132), module, Harmony::m3));
        addOutput(createOutput<PJ301MPort>(Vec(134, 132), module, Harmony::M3));
        addOutput(createOutput<PJ301MPort>(Vec(20, 197), module, Harmony::P4));
        addOutput(createOutput<PJ301MPort>(Vec(58, 197), module, Harmony::TT));
        addOutput(createOutput<PJ301MPort>(Vec(96, 197), module, Harmony::P5));
        addOutput(createOutput<PJ301MPort>(Vec(134, 197), module, Harmony::m6));
        addOutput(createOutput<PJ301MPort>(Vec(20, 262), module, Harmony::M6));
        addOutput(createOutput<PJ301MPort>(Vec(58, 262), module, Harmony::m7));
        addOutput(createOutput<PJ301MPort>(Vec(96, 262), module, Harmony::M7));
        addOutput(createOutput<PJ301MPort>(Vec(134, 262), module, Harmony::P8));
    }
};

Model *modelHarmony = createModel<Harmony, HarmonyWidget>("Harmony");
