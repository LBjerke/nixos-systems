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


struct Multimulti : Module
{
    enum ParamIds
    {
        NUM_PARAMS
    };
    enum InputIds
    {
        A1_INPUT,
        A2_INPUT,
        B1_INPUT,
        B2_INPUT,
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
        A7_OUTPUT,
        A8_OUTPUT,
        B1_OUTPUT,
        B2_OUTPUT,
        B3_OUTPUT,
        B4_OUTPUT,
        B5_OUTPUT,
        B6_OUTPUT,
        B7_OUTPUT,
        B8_OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Multimulti()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
    }
    void process(const ProcessArgs &args) override;
};


void Multimulti::process(const ProcessArgs &args)
{
    float in1 = inputs[A1_INPUT].getVoltage() + inputs[A2_INPUT].getVoltage();
    float in2 = inputs[B1_INPUT].getVoltage() + inputs[B2_INPUT].getVoltage();

    outputs[A1_OUTPUT].setVoltage(in1);
    outputs[A2_OUTPUT].setVoltage(in1);
    outputs[A3_OUTPUT].setVoltage(in1);
    outputs[A4_OUTPUT].setVoltage(in1);
    outputs[A5_OUTPUT].setVoltage(in1);
    outputs[A6_OUTPUT].setVoltage(in1);
    outputs[A7_OUTPUT].setVoltage(in1);
    outputs[A8_OUTPUT].setVoltage(in1);
    outputs[B1_OUTPUT].setVoltage(in2);
    outputs[B2_OUTPUT].setVoltage(in2);
    outputs[B3_OUTPUT].setVoltage(in2);
    outputs[B4_OUTPUT].setVoltage(in2);
    outputs[B5_OUTPUT].setVoltage(in2);
    outputs[B6_OUTPUT].setVoltage(in2);
    outputs[B7_OUTPUT].setVoltage(in2);
    outputs[B8_OUTPUT].setVoltage(in2);
}

struct MultimultiWidget : ModuleWidget
{
    MultimultiWidget(Multimulti *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/multimulti.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(18, 67), module, Multimulti::A1_INPUT));
        addInput(createInput<PJ301MPort>(Vec(78, 67), module, Multimulti::A2_INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(3, 125), module, Multimulti::A1_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(33, 125), module, Multimulti::A2_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(63, 125), module, Multimulti::A3_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(93, 125), module, Multimulti::A4_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(3, 155), module, Multimulti::A5_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(33, 155), module, Multimulti::A6_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(63, 155), module, Multimulti::A7_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(93, 155), module, Multimulti::A8_OUTPUT));

        addInput(createInput<PJ301MPort>(Vec(18, 227), module, Multimulti::B1_INPUT));
        addInput(createInput<PJ301MPort>(Vec(78, 227), module, Multimulti::B2_INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(3, 285), module, Multimulti::B1_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(33, 285), module, Multimulti::B2_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(63, 285), module, Multimulti::B3_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(93, 285), module, Multimulti::B4_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(3, 315), module, Multimulti::B5_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(33, 315), module, Multimulti::B6_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(63, 315), module, Multimulti::B7_OUTPUT));
        addOutput(createOutput<PJ301MPort>(Vec(93, 315), module, Multimulti::B8_OUTPUT));
    }
};

Model *modelMultimulti = createModel<Multimulti, MultimultiWidget>("Multimulti");
