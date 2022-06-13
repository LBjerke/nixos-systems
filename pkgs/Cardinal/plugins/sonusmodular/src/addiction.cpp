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

struct Addiction : Module
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

    float def_amps[4] = {1.0, 0.7, 0.0, 0.8};

    Addiction()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Addiction::OCTAVE, -3.0, 3.0, 0.0, "");

        for (unsigned int a = 1; a < Addiction::NUM_PARAMS; a++)
        {
            configParam(a, 0.0, 1.0, def_amps[(int)(a % 4)], "");
        }


    }

    void process(const ProcessArgs &args) override;

    float amp_sum = 0.0;
    float ramps[16] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float coeff[16] = {1.0, 1.2, 1.25, 1.5, 2.0, 2.4, 2.5, 3.0, 3.0, 3.6, 3.75, 4.5, 4.0, 4.8, 5.0, 6.0};

};


void Addiction::process(const ProcessArgs &args)
{
    float pitch = params[OCTAVE].getValue();
    pitch += inputs[CV_FREQ].getVoltage();
    pitch = clamp(pitch, -4.0, 4.0);
    float freq = 261.626 * powf(2.0, pitch);

    float inv_sample_rate = 1.0 / args.sampleRate;

    outputs[OUTPUT].setVoltage(0.0);
    amp_sum = 0.0;

    for (unsigned int i = 0; i < 16; i++)
    {
        ramps[i] += freq * inv_sample_rate * coeff[i];

        if (ramps[i] > 1.0)
        {
            ramps[i] = -1.0;
        }

        outputs[OUTPUT].value += sin(ramps[i] * M_PI) * (params[i + 1].getValue() + (inputs[i + 1].getVoltage() * 0.2));
        amp_sum += (params[i + 1].getValue() + (inputs[i + 1].getVoltage() * 0.2));
    }

    if (amp_sum > 0.0)
    {
        outputs[OUTPUT].value /= (amp_sum * 0.2);
    }
    else
    {
        outputs[OUTPUT].setVoltage(0.0);
    }
}

struct AddictionWidget : ModuleWidget
{
    AddictionWidget(Addiction *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/addiction.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addParam(createParam<SonusKnob>(Vec(20, 64), module, Addiction::OCTAVE));
        addInput(createInput<PJ301MPort>(Vec(25.5, 137), module, Addiction::CV_FREQ));

        addOutput(createOutput<PJ301MPort>(Vec(25.5, 277), module, Addiction::OUTPUT));

        for (unsigned int a = 0; a < Addiction::NUM_PARAMS - 1; a++)
        {
            addParam(createParam<SonusKnob>(Vec(150 + (50 * (a % 4)), 64 + (70 * floorf(a * 0.25))), module, a + 1));
            addInput(createInput<PJ301MPort>(Vec(155.5 + (50 * (a % 4)), 102 + (70 * floorf(a * 0.25))), module, a + 1));
        }
    }
};

Model *modelAddiction = createModel<Addiction, AddictionWidget>("Addiction");
