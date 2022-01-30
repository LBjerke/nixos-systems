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


struct Ladrone : Module
{
    enum ParamIds
    {
        OCTAVE,
        H1_I1,
        H1_I2,
        H1_I3,
        H1_I4,
        H2_I1,
        H2_I2,
        H2_I3,
        H2_I4,
        H3_I1,
        H3_I2,
        H3_I3,
        H3_I4,
        H4_I1,
        H4_I2,
        H4_I3,
        H4_I4,
        NUM_PARAMS
    };
    enum InputIds
    {
        CV_FREQ,
        CV_H1_I1,
        CV_H1_I2,
        CV_H1_I3,
        CV_H1_I4,
        CV_H2_I1,
        CV_H2_I2,
        CV_H2_I3,
        CV_H2_I4,
        CV_H3_I1,
        CV_H3_I2,
        CV_H3_I3,
        CV_H3_I4,
        CV_H4_I1,
        CV_H4_I2,
        CV_H4_I3,
        CV_H4_I4,
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

    Ladrone()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Ladrone::OCTAVE, -3.0, 3.0, -1.0, "");

        for (unsigned int a = 1; a < Ladrone::NUM_PARAMS; a++)
        {
            configParam(a, 0.0, 1.0, 1.0 - ((a % 4) * 0.2), "");
        }

    }

    void process(const ProcessArgs &args) override;

    float amp_sum = 0.0;
    float ramp_1[4] = {0.0, 0.0, 0.0, 0.0};
    float ramp_2[4] = {0.0, 0.0, 0.0, 0.0};
    float ramp_3[4] = {0.0, 0.0, 0.0, 0.0};
    float ramp_4[4] = {0.0, 0.0, 0.0, 0.0};
};


void Ladrone::process(const ProcessArgs &args)
{

    float pitch = params[OCTAVE].getValue();
    pitch += inputs[CV_FREQ].getVoltage();
    pitch = clamp(pitch, -4.0, 4.0);
    float freq = 261.626 * powf(2.0, pitch);

    float inv_sample_rate = 1.0 / args.sampleRate;

    for (unsigned int i = 0; i < 4; i++)
    {
        ramp_1[i] += freq * inv_sample_rate * (i + 1);
        if (ramp_1[i] > 1.0)
        {
            ramp_1[i] = -1.0;
        }

        ramp_2[i] += freq * inv_sample_rate * (i + 1) * 2.0;
        if (ramp_2[i] > 1.0)
        {
            ramp_2[i] = -1.0;
        }

        ramp_3[i] += freq * inv_sample_rate * (i + 1) * 3.0;
        if (ramp_3[i] > 1.0)
        {
            ramp_3[i] = -1.0;
        }

        ramp_4[i] += freq * inv_sample_rate * (i + 1) * 4.0;
        if (ramp_4[i] > 1.0)
        {
            ramp_4[i] = -1.0;
        }
    }

    outputs[OUTPUT].setVoltage(0.0);
    amp_sum = 0.0;

    outputs[OUTPUT].value += sin(ramp_1[0] * M_PI) * (params[H1_I1].getValue() + (inputs[CV_H1_I1].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_1[0] * ramp_1[1] * M_PI) * (params[H1_I2].getValue() + (inputs[CV_H1_I2].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_1[0] * ramp_1[2] * M_PI) * (params[H1_I3].getValue() + (inputs[CV_H1_I3].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_1[0] * ramp_1[3] * M_PI) * (params[H1_I4].getValue() + (inputs[CV_H1_I4].getVoltage() * 0.2));

    outputs[OUTPUT].value += sin(ramp_2[0] * M_PI) * (params[H2_I1].getValue() + (inputs[CV_H2_I1].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_2[0] * ramp_2[1] * M_PI) * (params[H2_I2].getValue() + (inputs[CV_H2_I2].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_2[0] * ramp_2[2] * M_PI) * (params[H2_I3].getValue() + (inputs[CV_H2_I3].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_2[0] * ramp_2[3] * M_PI) * (params[H2_I4].getValue() + (inputs[CV_H2_I4].getVoltage() * 0.2));

    outputs[OUTPUT].value += sin(ramp_3[0] * M_PI) * (params[H3_I1].getValue() + (inputs[CV_H1_I1].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_3[0] * ramp_3[1] * M_PI) * (params[H3_I2].getValue() + (inputs[CV_H3_I2].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_3[0] * ramp_3[2] * M_PI) * (params[H3_I3].getValue() + (inputs[CV_H3_I3].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_3[0] * ramp_3[3] * M_PI) * (params[H3_I4].getValue() + (inputs[CV_H3_I4].getVoltage() * 0.2));

    outputs[OUTPUT].value += sin(ramp_4[0] * M_PI) * (params[H4_I1].getValue() + (inputs[CV_H4_I1].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_4[0] * ramp_4[1] * M_PI) * (params[H4_I2].getValue() + (inputs[CV_H4_I2].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_4[0] * ramp_4[2] * M_PI) * (params[H4_I3].getValue() + (inputs[CV_H4_I3].getVoltage() * 0.2));
    outputs[OUTPUT].value += sin(ramp_4[0] * ramp_4[3] * M_PI) * (params[H4_I4].getValue() + (inputs[CV_H4_I4].getVoltage() * 0.2));

    for (int o = H1_I1; o < NUM_PARAMS; o++)
    {
        amp_sum += (params[o].getValue() + (inputs[o].getVoltage() * 0.2));
    }

    amp_sum > 0.0 ? outputs[OUTPUT].setVoltage(outputs[OUTPUT].value / (amp_sum * 0.2)) : outputs[OUTPUT].setVoltage(0.0);
}

struct LadroneWidget : ModuleWidget
{
    LadroneWidget(Ladrone *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/ladrone.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addParam(createParam<SonusKnob>(Vec(20, 64), module, Ladrone::OCTAVE));
        addInput(createInput<PJ301MPort>(Vec(25.5, 137), module, Ladrone::CV_FREQ));

        addOutput(createOutput<PJ301MPort>(Vec(25.5, 277), module, Ladrone::OUTPUT));

        for (unsigned int a = 0; a < Ladrone::NUM_PARAMS - 1; a++)
        {
            addParam(createParam<SonusKnob>(Vec(150 + (50 * (a % 4)), 64 + (70 * floorf(a * 0.25))), module, a + 1));
            addInput(createInput<PJ301MPort>(Vec(155.5 + (50 * (a % 4)), 102 + (70 * floorf(a * 0.25))), module, a + 1));
        }
    }
};

Model *modelLadrone = createModel<Ladrone, LadroneWidget>("Ladrone");
