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
#include "wtnet.h"

struct Neurosc : Module
{
    enum ParamIds
    {
        OCTAVE,
        L1,
        L2,
        L3,
        L4,
        L5,
        L6,
        L7,
        L8,
        NUM_PARAMS
    };
    enum InputIds
    {
        CV_FREQ,
        CV_L1,
        CV_L2,
        CV_L3,
        CV_L4,
        CV_L5,
        CV_L6,
        CV_L7,
        CV_L8,
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

    Neurosc()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Neurosc::OCTAVE, -3.0, 3.0, 0.0, "");

        for (unsigned int a = 1; a < Neurosc::NUM_PARAMS; a++)
        {
            configParam(a, 0.0, 10.0, 5.0, "");
        }
    }

    void process(const ProcessArgs &args) override;

    float amp_sum = 0.0;
    float ramp = 0.0;
    float out = 0.0;
    unsigned int inp[2] = {0, 1};
    float inf = 0.0;
    std::vector<float> wave = std::vector<float>(600, 0.0f);
    float inv_sample_rate;
    float coeff[8] = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0, 5.0};

};


void Neurosc::process(const ProcessArgs &args)
{
    float pitch = params[OCTAVE].getValue();
    pitch += inputs[CV_FREQ].getVoltage();
    pitch = clamp(pitch, -4.0, 4.0);
    float freq = 261.626 * powf(2.0, pitch);

    inv_sample_rate = 1.0 / args.sampleRate;

    ramp += freq * inv_sample_rate * 599.0;

    if (ramp > 599.0)
    {
        ramp = 0.0;

        for (unsigned int l = 0; l < 8; l++)
        {
            coeff[l] = params[l + 1].getValue() + (inputs[l + 1].getVoltage());
            coeff[l] = clamp(coeff[l], 0.0, 10.0);
        }
        generate_wave(coeff, wave, true);
    }

    inp[0] = (unsigned int)(floorf(ramp));
    inp[1] = (inp[0] + 1) % 600;
    inf = float(inp[0] + 1) - ramp;

    out = (wave.at(inp[0]) * inf) +  (wave.at(inp[1]) * (1.0 - inf));

    outputs[OUTPUT].setVoltage(out * 5.0);
}

struct NeuroscWidget : ModuleWidget
{
    NeuroscWidget(Neurosc *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/neurosc.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addParam(createParam<SonusKnob>(Vec(20, 64), module, Neurosc::OCTAVE));
        addInput(createInput<PJ301MPort>(Vec(25.5, 137), module, Neurosc::CV_FREQ));

        addOutput(createOutput<PJ301MPort>(Vec(25.5, 277), module, Neurosc::OUTPUT));

        for (unsigned int a = 0; a < Neurosc::NUM_PARAMS - 1; a++)
        {
            if (a < 4)
            {
                addParam(createParam<SonusKnob>(Vec(100, 64 + (70 * (a % 4))), module, a + 1));
                addInput(createInput<PJ301MPort>(Vec(105.5, 102 + (70 * (a % 4))), module, a + 1));
            }
            else
            {
                addParam(createParam<SonusKnob>(Vec(150, 64 + (70 * (a % 4))), module, a + 1));
                addInput(createInput<PJ301MPort>(Vec(155.5, 102 + (70 * (a % 4))), module, a + 1));
            }
        }
    }
};

Model *modelNeurosc = createModel<Neurosc, NeuroscWidget>("Neurosc");
