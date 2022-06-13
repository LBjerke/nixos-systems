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


struct Scramblase : Module
{
    enum ParamIds
    {
        THRESHOLD,
        NUM_PARAMS
    };
    enum InputIds
    {
        IN_A,
        IN_B,
        IN_C,
        THRESHOLD_CV,
        NUM_INPUTS
    };
    enum OutputIds
    {
        OUT_A1,
        OUT_A2,
        OUT_A3,
        OUT_A4,
        OUT_B1,
        OUT_B2,
        OUT_B3,
        OUT_B4,
        OUT_C1,
        OUT_C2,
        OUT_C3,
        OUT_C4,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Scramblase()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Scramblase::THRESHOLD, 0.0, 1.0, 1.0, "");

    }
    void process(const ProcessArgs &args) override;
    float threshold = 0.0;
};


void Scramblase::process(const ProcessArgs &args)
{
    float in_a = inputs[IN_A].getVoltage() / 5.0;
    float in_b = inputs[IN_B].getVoltage() / 5.0;
    float in_c = inputs[IN_C].getVoltage() / 5.0;

    float out_a1, out_a2, out_a3, out_a4;
    float out_b1, out_b2, out_b3, out_b4;
    float out_c1, out_c2, out_c3, out_c4;

    threshold = params[THRESHOLD].getValue() + inputs[THRESHOLD_CV].getVoltage() / 5.0;
    if (threshold > 1.0)
    {
        threshold = 1.0;
    }
    if (threshold < 0.0)
    {
        threshold = 0.0;
    }

    if (fabs(in_a) > threshold)
    {
        out_a1 = (fabs(in_a) - 2.0 * (fabs(in_a) - threshold)) * copysign(1.0, in_a);
        out_a2 = threshold * copysign (1.0, in_a);
        out_a3 = 1.0 * copysign (1.0, in_a);
        out_a4 = (fabs(in_a) - 2.0 * (fabs(in_a) - threshold)) * copysign(1.0, in_a);
    }
    else
    {
        out_a1 = in_a;
        out_a2 = in_a;
        out_a3 = in_a;
        out_a4 = 1.0 * copysign(1.0, in_a);
    }

    if (fabs(in_b) > threshold)
    {
        out_b1 = (fabs(in_b) - 2.0 * (fabs(in_b) - threshold)) * copysign(1.0, in_b);
        out_b2 = threshold * copysign (1.0, in_b);
        out_b3 = 1.0 * copysign (1.0, in_b);
        out_b4 = (fabs(in_b) - 2.0 * (fabs(in_b) - threshold)) * copysign(1.0, in_b);
    }
    else
    {
        out_b1 = in_b;
        out_b2 = in_b;
        out_b3 = in_b;
        out_b4 = 1.0 * copysign(1.0, in_b);
    }

    if (fabs(in_c) > threshold)
    {
        out_c1 = (fabs(in_c) - 2.0 * (fabs(in_c) - threshold)) * copysign(1.0, in_c);
        out_c2 = threshold * copysign (1.0, in_c);
        out_c3 = 1.0 * copysign (1.0, in_c);
        out_c4 = (fabs(in_c) - 2.0 * (fabs(in_c) - threshold)) * copysign(1.0, in_c);
    }
    else
    {
        out_c1 = in_c;
        out_c2 = in_c;
        out_c3 = in_c;
        out_c4 = 1.0 * copysign(1.0, in_c);
    }

    outputs[OUT_A1].setVoltage(out_a1 * 5.0);
    outputs[OUT_A2].setVoltage(out_a2 * 5.0);
    outputs[OUT_A3].setVoltage(out_a3 * 5.0);
    outputs[OUT_A4].setVoltage(out_a4 * 5.0);

    outputs[OUT_B1].setVoltage(out_b1 * 5.0);
    outputs[OUT_B2].setVoltage(out_b2 * 5.0);
    outputs[OUT_B3].setVoltage(out_b3 * 5.0);
    outputs[OUT_B4].setVoltage(out_b4 * 5.0);

    outputs[OUT_C1].setVoltage(out_c1 * 5.0);
    outputs[OUT_C2].setVoltage(out_c2 * 5.0);
    outputs[OUT_C3].setVoltage(out_c3 * 5.0);
    outputs[OUT_C4].setVoltage(out_c4 * 5.0);
}

struct ScramblaseWidget : ModuleWidget
{
    ScramblaseWidget(Scramblase *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/scramblase.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(12, 67), module, Scramblase::IN_A));
        addOutput(createOutput<PJ301MPort>(Vec(12, 121), module, Scramblase::OUT_A1));
        addOutput(createOutput<PJ301MPort>(Vec(12, 150), module, Scramblase::OUT_A2));
        addOutput(createOutput<PJ301MPort>(Vec(12, 179), module, Scramblase::OUT_A3));
        addOutput(createOutput<PJ301MPort>(Vec(12, 208), module, Scramblase::OUT_A4));

        addInput(createInput<PJ301MPort>(Vec(47, 67), module, Scramblase::IN_B));
        addOutput(createOutput<PJ301MPort>(Vec(47, 120), module, Scramblase::OUT_B1));
        addOutput(createOutput<PJ301MPort>(Vec(47, 150), module, Scramblase::OUT_B2));
        addOutput(createOutput<PJ301MPort>(Vec(47, 179), module, Scramblase::OUT_B3));
        addOutput(createOutput<PJ301MPort>(Vec(47, 208), module, Scramblase::OUT_B4));

        addInput(createInput<PJ301MPort>(Vec(83, 67), module, Scramblase::IN_C));
        addOutput(createOutput<PJ301MPort>(Vec(83, 121), module, Scramblase::OUT_C1));
        addOutput(createOutput<PJ301MPort>(Vec(83, 150), module, Scramblase::OUT_C2));
        addOutput(createOutput<PJ301MPort>(Vec(83, 179), module, Scramblase::OUT_C3));
        addOutput(createOutput<PJ301MPort>(Vec(83, 208), module, Scramblase::OUT_C4));

        addInput(createInput<PJ301MPort>(Vec(12, 290), module, Scramblase::THRESHOLD_CV));

        addParam(createParam<SonusBigKnob>(Vec(53, 275), module, Scramblase::THRESHOLD));
    }
};

Model *modelScramblase = createModel<Scramblase, ScramblaseWidget>("Scramblase");
