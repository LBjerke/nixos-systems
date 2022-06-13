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


struct Chainsaw : Module
{
    enum ParamIds
    {
        OCTAVE,
        OSC1_SHAPE,
        OSC1_TUNE,
        OSC2_SHAPE,
        OSC2_TUNE,
        OSC3_SHAPE,
        OSC3_TUNE,
        ALIGN,
        NUM_PARAMS
    };
    enum InputIds
    {
        PITCH,
        CV_OSC1_SHAPE,
        CV_OSC1_TUNE,
        CV_OSC2_SHAPE,
        CV_OSC2_TUNE,
        CV_OSC3_SHAPE,
        CV_OSC3_TUNE,
        CV_ALIGN,
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

    Chainsaw()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Chainsaw::OCTAVE, -3.0, 3.0, 0.0, "");
        configParam(Chainsaw::OSC1_SHAPE, -1.0, 1.0, 0.0, "");
        configParam(Chainsaw::OSC1_TUNE, -1.0, 1.0, 0.0, "");
        configParam(Chainsaw::OSC2_SHAPE, -1.0, 1.0, 0.0, "");
        configParam(Chainsaw::OSC2_TUNE, -1.0, 1.0, 0.0, "");
        configParam(Chainsaw::OSC3_SHAPE, -1.0, 1.0, 0.0, "");
        configParam(Chainsaw::OSC3_TUNE, -1.0, 1.0, 0.0, "");
        configParam(Chainsaw::ALIGN, 0.0, 1.0, 0.0, "");

    }
    void process(const ProcessArgs &args) override;

    float osc1_phase = 0.0;
    float osc2_phase = 0.0;
    float osc3_phase = 0.0;

    float osc_wave1_mix[3] = {0.0, 0.0, 0.0};
    float osc_wave2_mix[3] = {0.0, 0.0, 0.0};
    float osc_wave3_mix[3] = {0.0, 0.0, 0.0};
    float osc[3] = {0.0, 0.0, 0.0};
    float osc_freq[3] = {0.0, 0.0, 0.0};
    float osc_phase[3] = {0.0, 0.0, 0.0};
};


void Chainsaw::process(const ProcessArgs &args)
{
    float pitch = params[OCTAVE].getValue();
    pitch += inputs[PITCH].getVoltage();
    pitch = clamp(pitch, -4.0, 4.0);

    for (unsigned int f = 0; f < 3; f++)
    {
        osc_freq[f] = 261.626 * powf(2.0, pitch + (inputs[(f * 2) + 2].getVoltage() / 5.0) + params[(f * 2) + 2].getValue());
    }

    for (unsigned int p = 0; p < 3; p++)
    {
        osc_phase[p] += 2.0 * (osc_freq[p] / args.sampleRate);
        if (osc_phase[p] >= 1.0)
        {
            osc_phase[p] -= 2.0;
        }
    }

    for (unsigned int o = 0; o < 3; o++)
    {
        osc_wave1_mix[o] = (-1.0 * params[(o * 2) + 1].getValue()) + (inputs[(o * 2) + 1].getVoltage() / -5.0);
        osc_wave1_mix[o] = clamp(osc_wave1_mix[o], 0.0, 1.0);

        osc_wave3_mix[o] = params[(o * 2) + 1].getValue() + (inputs[(o * 2) + 1].getVoltage() / 5.0);
        osc_wave3_mix[o] = clamp(osc_wave3_mix[o], 0.0, 1.0);

        osc_wave2_mix[o] = (2.0 - osc_wave1_mix[o] - osc_wave3_mix[o]) * 0.5;
    }

    for (unsigned int i = 0; i < 3; i++)
    {
        osc[i] = ((osc_wave1_mix[i] * powf(osc_phase[i], 4.0)) - (0.2 * osc_wave1_mix[i])) + (osc_wave2_mix[i] * osc_phase[i]) + (osc_wave3_mix[i] * 0.8 * cbrt(cbrt(osc_phase[i])));
    }

    outputs[OUTPUT].setVoltage((osc[0] + osc[1] + osc[2]) * (5.0 / 3.0));

    if (params[ALIGN].getValue() != 0.0 || inputs[CV_ALIGN].getVoltage() != 0)
    {
        osc_phase[1] = osc_phase[0];
        osc_phase[2] = osc_phase[0];
    }
}

struct ChainsawWidget : ModuleWidget
{
    ChainsawWidget(Chainsaw *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/chainsaw.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Chainsaw::PITCH));
        addInput(createInput<PJ301MPort>(Vec(14, 132), module, Chainsaw::CV_OSC1_SHAPE));
        addInput(createInput<PJ301MPort>(Vec(143, 132), module, Chainsaw::CV_OSC1_TUNE));
        addInput(createInput<PJ301MPort>(Vec(14, 197), module, Chainsaw::CV_OSC2_SHAPE));
        addInput(createInput<PJ301MPort>(Vec(143, 197), module, Chainsaw::CV_OSC2_TUNE));
        addInput(createInput<PJ301MPort>(Vec(14, 262), module, Chainsaw::CV_OSC3_SHAPE));
        addInput(createInput<PJ301MPort>(Vec(143, 262), module, Chainsaw::CV_OSC3_TUNE));
        addInput(createInput<PJ301MPort>(Vec(100, 324), module, Chainsaw::CV_ALIGN));

        addOutput(createOutput<PJ301MPort>(Vec(143, 67), module, Chainsaw::OUTPUT));

        addParam(createParam<SonusKnob>(Vec(72, 65), module, Chainsaw::OCTAVE));
        addParam(createParam<SonusKnob>(Vec(50, 128), module, Chainsaw::OSC1_SHAPE));
        addParam(createParam<SonusKnob>(Vec(94, 128), module, Chainsaw::OSC1_TUNE));
        addParam(createParam<SonusKnob>(Vec(50, 193), module, Chainsaw::OSC2_SHAPE));
        addParam(createParam<SonusKnob>(Vec(94, 193), module, Chainsaw::OSC2_TUNE));
        addParam(createParam<SonusKnob>(Vec(50, 258), module, Chainsaw::OSC3_SHAPE));
        addParam(createParam<SonusKnob>(Vec(94, 258), module, Chainsaw::OSC3_TUNE));
        addParam(createParam<CKD6>(Vec(67,322), module, Chainsaw::ALIGN));
    }
};

Model *modelChainsaw = createModel<Chainsaw, ChainsawWidget>("Chainsaw");
