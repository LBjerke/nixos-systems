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


struct Pith : Module
{
    enum ParamIds
    {
        DELAY,
        NUM_PARAMS
    };
    enum InputIds
    {
        INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        L_OUTPUT,
        R_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Pith()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Pith::DELAY, 0.025, 10.0, 5.0125, "");
    }
    void process(const ProcessArgs &args) override;

    std::deque<float> buffer{std::deque<float>(4096, 0.0)};
    unsigned int current_sample = 0;
};

void Pith::process(const ProcessArgs &args)
{
    float in = inputs[INPUT].getVoltage();

    buffer.at(current_sample) = in;

    unsigned int right_sample = (current_sample + (unsigned int)floorf(params[DELAY].getValue() * args.sampleRate * 0.001)) % 4096;

    outputs[L_OUTPUT].setVoltage(in);
    outputs[R_OUTPUT].setVoltage(buffer.at(right_sample));

    if (++current_sample >= 4096)
    {
        current_sample = 0;
    }
}

struct PithWidget : ModuleWidget
{
    PithWidget(Pith *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/pith.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(33.3, 66), module, Pith::INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(13.2, 266), module, Pith::L_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(51.2, 266), module, Pith::R_OUTPUT));

        addParam(createParam<SonusBigKnob>(Vec(19, 155), module, Pith::DELAY));
    }
};

Model *modelPith = createModel<Pith, PithWidget>("PanintheHaas");
