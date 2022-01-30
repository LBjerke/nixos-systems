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


struct Deathcrush : Module
{
    enum ParamIds
    {
        DRIVE1,
        DRIVE2,
        BITS,
        NUM_PARAMS
    };
    enum InputIds
    {
        INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Deathcrush()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Deathcrush::DRIVE1, 0.0, 1.0, 0.0, "");
        configParam(Deathcrush::DRIVE2, 0.0, 1.0, 0.0, "");
        configParam(Deathcrush::BITS, 1.0, 12.0, 12.0, "");
    }

    void process(const ProcessArgs &args) override;
};


void Deathcrush::process(const ProcessArgs &args)
{
    float in = inputs[INPUT].getVoltage() / 5.0;
    float out = 0.0;
    float drive1_amount = params[DRIVE1].getValue();
    float drive2_amount = params[DRIVE2].getValue();
    float bits = params[BITS].getValue();

    out = (in * (1.0 - drive1_amount)) + ((copysign(1.0, in) * tan(powf(fabs(in), 0.25)) * drive1_amount * 0.75));

    if (fabs(in) > (1.0 - drive2_amount))
    {
        out = (out * (1.0 - drive2_amount)) + (copysign(1.0, in) * drive2_amount);
    }

    out = out * powf(2.0, bits - 1.0);
    out = round(out);
    out = out / powf(2.0, bits - 1.0);

    outputs[OUTPUT].setVoltage(out * 5.0);
}

struct DeathcrushWidget : ModuleWidget
{
    DeathcrushWidget(Deathcrush *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/deathcrush.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Deathcrush::INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(52, 67), module, Deathcrush::OUTPUT));

        addParam(createParam<SonusKnob>(Vec(27, 150), module, Deathcrush::DRIVE1));
        addParam(createParam<SonusKnob>(Vec(27, 210), module, Deathcrush::DRIVE2));
        addParam(createParam<SonusBigKnob>(Vec(18, 275), module, Deathcrush::BITS));
    }
};

Model *modelDeathcrush = createModel<Deathcrush, DeathcrushWidget>("Deathcrush");
