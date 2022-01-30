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


struct Mrcheb : Module
{
    enum ParamIds
    {
        S1K,
        S2K,
        S3K,
        S4K,
        S5K,
        S6K,
        S7K,
        S8K,
        S9K,
        RANGE,
        NUM_PARAMS
    };
    enum InputIds
    {
        INPUT,
        NUM_INPUTS
    };
    enum OutputIds
    {
        S1,
        S2,
        S3,
        S4,
        S5,
        S6,
        S7,
        S8,
        S9,
        OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Mrcheb()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Mrcheb::S1K, 0.0, 1.0, 0.2, "");
        configParam(Mrcheb::S2K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S3K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S4K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S5K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S6K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S7K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S8K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::S9K, 0.0, 1.0, 0.1, "");
        configParam(Mrcheb::RANGE, 0.0, 1.0, 1.0, "");

    }
    void process(const ProcessArgs &args) override;
};

bool chebyshev_poly(const float &input, float &output, unsigned int number = 1)
{
    output = 0.0;

    switch (number)
    {
    case 0:
        output = 1.0;
        break;
    case 1:
        output = input;
        break;
    case 2:
        output = 2.0 * pow(input, 2.0) - 1.0;
        break;
    case 3:
        output = 4.0 * pow(input, 3.0) - 3.0 * input;
        break;
    case 4:
        output = 8.0 * pow(input, 4.0) - 8.0 * pow(input, 2.0) + 1.0;
        break;
    case 5:
        output = 16.0 * pow(input, 5.0) - 20.0 * pow(input, 3.0) + 5.0 * input;
        break;
    case 6:
        output = 32.0 * pow(input, 6.0) - 48.0 * pow(input, 4.0) + 18.0 * pow(input, 2.0) - 1.0;
        break;
    case 7:
        output = 64.0 * pow(input, 7.0) - 112.0 * pow(input, 5.0) + 56.0 * pow(input, 3.0) - 7.0 * input;
        break;
    case 8:
        output = 128.0 * pow(input, 8.0) - 256.0 * pow(input, 6.0) + 160.0 * pow(input, 4.0) - 32.0 * pow(input, 2.0) + 1.0;
        break;
    case 9:
        output = 256.0 * pow(input, 9.0) - 576.0 * pow(input, 7.0) + 432.0 * pow(input, 5.0) - 120.0 * pow(input, 3.0) + 9.0 * input;
        break;
    case 10:
        output = 512 * pow(input, 10.0) - 1280 * pow(input, 8.0) + 1120 * pow(input, 6.0) - 400.0 * pow(input, 4.0) + 50.0 * pow(input, 2.0) - 1.0;
        break;
    }

    return true;
}


void Mrcheb::process(const ProcessArgs &args)
{
    float input = inputs[INPUT].getVoltage();
    float outs[9];
    float output = 0.0;
    float mix_sum = 0.0;
    bool bipolar = true;

    if (params[RANGE].getValue() == 0.0)
    {
        bipolar = false;
    }

    if (bipolar)
    {
        if (input < -5.0)
        {
            input = -5.0;
        }
        if (input > 5.0)
        {
            input = 5.0;
        }

        input *= 0.2;
    }
    else
    {
        if (input < 0.0)
        {
            input = 0.0;
        }
        if (input > 10.0)
        {
            input = 10.0;
        }

        input *= 0.1;
    }

    for (int s = 0; s < 9; s++)
    {
        chebyshev_poly(input, outs[s], s + 1);
    }

    for (int s = 0; s < 9; s++)
    {
        bipolar ? outputs[s].setVoltage(outs[s] * 5.0) : outputs[s].setVoltage(outs[s] * 10.0);
    }

    for (int k = 0; k < 9; k++)
    {
        output += outputs[k].value * params[k].getValue();
        mix_sum += params[k].getValue();
    }

    mix_sum > 1.0 ? outputs[OUTPUT].setVoltage(output / mix_sum) : outputs[OUTPUT].setVoltage(output);
}

struct MrchebWidget : ModuleWidget
{
    MrchebWidget(Mrcheb *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/mrcheb.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(58, 67), module, Mrcheb::INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(222.7, 67), module, Mrcheb::OUTPUT));

        addOutput(createOutput<PJ301MPort>(Vec(20, 132), module, Mrcheb::S1));
        addOutput(createOutput<PJ301MPort>(Vec(58, 132), module, Mrcheb::S2));
        addOutput(createOutput<PJ301MPort>(Vec(96, 132), module, Mrcheb::S3));
        addOutput(createOutput<PJ301MPort>(Vec(20, 197), module, Mrcheb::S4));
        addOutput(createOutput<PJ301MPort>(Vec(58, 197), module, Mrcheb::S5));
        addOutput(createOutput<PJ301MPort>(Vec(96, 197), module, Mrcheb::S6));
        addOutput(createOutput<PJ301MPort>(Vec(20, 262), module, Mrcheb::S7));
        addOutput(createOutput<PJ301MPort>(Vec(58, 262), module, Mrcheb::S8));
        addOutput(createOutput<PJ301MPort>(Vec(96, 262), module, Mrcheb::S9));

        addParam(createParam<SonusKnob>(Vec(178, 125), module, Mrcheb::S1K));
        addParam(createParam<SonusKnob>(Vec(216.5, 125), module, Mrcheb::S2K));
        addParam(createParam<SonusKnob>(Vec(255, 125), module, Mrcheb::S3K));
        addParam(createParam<SonusKnob>(Vec(178, 190), module, Mrcheb::S4K));
        addParam(createParam<SonusKnob>(Vec(216.5, 190), module, Mrcheb::S5K));
        addParam(createParam<SonusKnob>(Vec(255, 190), module, Mrcheb::S6K));
        addParam(createParam<SonusKnob>(Vec(178, 255), module, Mrcheb::S7K));
        addParam(createParam<SonusKnob>(Vec(216.5, 255), module, Mrcheb::S8K));
        addParam(createParam<SonusKnob>(Vec(255, 255), module, Mrcheb::S9K));

        addParam(createParam<CKSS>(Vec(127, 70), module, Mrcheb::RANGE));
    }
};

Model *modelMrcheb = createModel<Mrcheb, MrchebWidget>("Mrcheb");
