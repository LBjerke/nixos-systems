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


struct Piconoise : Module
{
    enum ParamIds
    {
        NUM_PARAMS
    };
    enum InputIds
    {
        NUM_INPUTS
    };
    enum OutputIds
    {
        A1_OUTPUT,
        A2_OUTPUT,
        A3_OUTPUT,
        A4_OUTPUT,
        A5_OUTPUT,
        A6_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Piconoise()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        srand(time(0));
    }
    void process(const ProcessArgs &args) override;
};


void Piconoise::process(const ProcessArgs &args)
{
    float out = (2.0 * (rand() / (float)RAND_MAX)) - 1.0;

    outputs[A1_OUTPUT].setVoltage(5.0 * out);
    outputs[A2_OUTPUT].setVoltage(5.0 * out);
    outputs[A3_OUTPUT].setVoltage(5.0 * out);
    outputs[A4_OUTPUT].setVoltage(5.0 * out);
    outputs[A5_OUTPUT].setVoltage(5.0 * out);
    outputs[A6_OUTPUT].setVoltage(5.0 * out);
}

struct PiconoiseWidget : ModuleWidget
{
    PiconoiseWidget(Piconoise *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/piconoise.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addOutput(createOutput<PJ301MPort>(Vec(18, 67), module, Piconoise::A1_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(18, 112), module, Piconoise::A2_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(18, 157), module, Piconoise::A3_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(18, 202), module, Piconoise::A4_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(18, 247), module, Piconoise::A5_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(18, 292), module, Piconoise::A6_OUTPUT));
    }
};

Model *modelPiconoise = createModel<Piconoise, PiconoiseWidget>("Piconoise");
