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


struct Bymidside : Module
{
    enum ParamIds
    {
        M_GAIN,
        S_GAIN,
        NUM_PARAMS
    };
    enum InputIds
    {
        L_INPUT,
        R_INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        M_OUTPUT,
        S_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Bymidside()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Bymidside::M_GAIN, 0.0, 2.0, 1.0, "");
        configParam(Bymidside::S_GAIN, 0.0, 2.0, 1.0, "");

    }
    void process(const ProcessArgs &args) override;
};


void Bymidside::process(const ProcessArgs &args)
{
    float mid = inputs[L_INPUT].getVoltage() + inputs[R_INPUT].getVoltage();
    float side = inputs[L_INPUT].getVoltage() - inputs[R_INPUT].getVoltage();

    float mid_gain = params[M_GAIN].getValue();

    float side_gain = params[S_GAIN].getValue();

    outputs[M_OUTPUT].setVoltage(mid * mid_gain);
    outputs[S_OUTPUT].setVoltage(side * side_gain);
}

struct BymidsideWidget : ModuleWidget
{
    BymidsideWidget(Bymidside *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/bymidside.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Bymidside::L_INPUT));
        addInput(createInput<PJ301MPort>(Vec(52, 67), module, Bymidside::R_INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(14, 132), module, Bymidside::M_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(52, 132), module, Bymidside::S_OUTPUT));

        addParam(createParam<SonusBigKnob>(Vec(18, 195), module, Bymidside::M_GAIN));
        addParam(createParam<SonusBigKnob>(Vec(18, 275), module, Bymidside::S_GAIN));
    }
};

Model *modelBymidside = createModel<Bymidside, BymidsideWidget>("Bymidside");
