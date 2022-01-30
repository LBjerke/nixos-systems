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


struct Twoff : Module
{
    enum ParamIds
    {
        OFF_A,
        OFF_B,
        NUM_PARAMS
    };
    enum InputIds
    {
        IN_A,
        IN_B,
        NUM_INPUTS
    };
    enum OutputIds
    {
        OUT_A,
        OUT_B,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Twoff()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Twoff::OFF_A, -5.0, 5.0, 0.0, "");
        configParam(Twoff::OFF_B, -5.0, 5.0, 0.0, "");

    }
    void process(const ProcessArgs &args) override;
};


void Twoff::process(const ProcessArgs &args)
{
    float in_a = inputs[IN_A].getVoltage();
    float in_b = inputs[IN_B].getVoltage();

    outputs[OUT_A].setVoltage(in_a + params[OFF_A].getValue());
    outputs[OUT_B].setVoltage(in_b + params[OFF_B].getValue());
}

struct TwoffWidget : ModuleWidget
{
    TwoffWidget(Twoff *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/twoff.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(18, 67), module, Twoff::IN_A));
        addInput(createInput<PJ301MPort>(Vec(18, 202), module, Twoff::IN_B));
        addOutput(createOutput<PJ301MPort>(Vec(18, 157), module, Twoff::OUT_A));
        addOutput(createOutput<PJ301MPort>(Vec(18, 292), module, Twoff::OUT_B));

        addParam(createParam<SonusKnob>(Vec(12, 99), module, Twoff::OFF_A));
        addParam(createParam<SonusKnob>(Vec(12, 235), module, Twoff::OFF_B));
    }
};

Model *modelTwoff = createModel<Twoff, TwoffWidget>("Twoff");
