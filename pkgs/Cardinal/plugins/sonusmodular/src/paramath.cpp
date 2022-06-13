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


struct Paramath : Module
{
    enum ParamIds
    {
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
        A_GEQ_B,
        A_EQ_B,
        MIN,
        MAX,
        A_MUL_B,
        PYTHAGORAS,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Paramath()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }
    void process(const ProcessArgs &args) override;
};


void Paramath::process(const ProcessArgs &args)
{
    float in_a = inputs[IN_A].getVoltage();
    float in_b = inputs[IN_B].getVoltage();

    if (in_a >= in_b)
    {
        outputs[A_GEQ_B].setVoltage(5.0);
        outputs[MIN].setVoltage(in_b);
        outputs[MAX].setVoltage(in_a);
    }
    else
    {
        outputs[A_GEQ_B].setVoltage(0.0);
        outputs[MIN].setVoltage(in_a);
        outputs[MAX].setVoltage(in_b);
    }

    if (in_a == in_b)
    {
        outputs[A_EQ_B].setVoltage(5.0);
    }
    else
    {
        outputs[A_EQ_B].setVoltage(0.0);
    }

    // These two value are computed on normalized [-1.0; 1.0]
    outputs[A_MUL_B].setVoltage((in_a * in_b) / 5.0);
    outputs[PYTHAGORAS].setVoltage(sqrt(powf(in_a / 5.0, 2.0) + powf(in_b / 5.0, 2.0)) * 5.0);
}

struct ParamathWidget : ModuleWidget
{
    ParamathWidget(Paramath *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/paramath.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Paramath::IN_A));
        addInput(createInput<PJ301MPort>(Vec(52, 67), module, Paramath::IN_B));

        addOutput(createOutput<PJ301MPort>(Vec(14, 132), module, Paramath::A_GEQ_B));
        addOutput(createOutput<PJ301MPort>(Vec(52, 132), module, Paramath::A_EQ_B));
        addOutput(createOutput<PJ301MPort>(Vec(14, 197), module, Paramath::MIN));
        addOutput(createOutput<PJ301MPort>(Vec(52, 197), module, Paramath::MAX));
        addOutput(createOutput<PJ301MPort>(Vec(14, 262), module, Paramath::A_MUL_B));
        addOutput(createOutput<PJ301MPort>(Vec(52, 262), module, Paramath::PYTHAGORAS));
    }
};

Model *modelParamath = createModel<Paramath, ParamathWidget>("Paramath");
