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


struct Oktagon : Module
{
    enum ParamIds
    {
        FREQUENCY,
        RANGE,
        NUM_PARAMS
    };
    enum InputIds
    {
        CV_FREQ,
        NUM_INPUTS
    };
    enum OutputIds
    {
        WAVE0_OUT,
        WAVE45_OUT,
        WAVE90_OUT,
        WAVE135_OUT,
        WAVE180_OUT,
        WAVE225_OUT,
        WAVE270_OUT,
        WAVE315_OUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        FREQ_LIGHT,
        NUM_LIGHTS
    };

    Oktagon()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Oktagon::RANGE, 0.0, 1.0, 0.0, "");
        configParam(Oktagon::FREQUENCY, -2.0, 2.0, 0.0, "");

    }
    void process(const ProcessArgs &args) override;

    float phase = 0.0;
    float light = 0.0;
    bool audio_range = false;
};


void Oktagon::process(const ProcessArgs &args)
{
    float freq = 0.0;

    if (params[RANGE].getValue() == 0.0)
    {
        audio_range = false;
    }
    else
    {
        audio_range = true;
    }

    if (!audio_range)
    {
        freq = powf(10.0, params[FREQUENCY].getValue()) * powf(10.0, inputs[CV_FREQ].getVoltage() / 5.0);
    }
    else
    {
        float pitch = params[FREQUENCY].getValue();
        pitch += inputs[CV_FREQ].getVoltage();
        pitch = clamp(pitch, -4.0, 4.0);
        freq = 261.626 * powf(2.0, pitch);
    }

    phase += freq / args.sampleRate;
    if (phase >= 1.0)
    {
        phase -= 1.0;
    }

    for (int w = 0; w < NUM_OUTPUTS; w++)
    {
        outputs[w].setVoltage(sinf(2.0 * M_PI * (phase + w * 0.125)) * 5.0);
    }

    lights[FREQ_LIGHT].value = (outputs[WAVE0_OUT].value > 0.0) ? 1.0 : 0.0;
}

struct OktagonWidget : ModuleWidget
{
    OktagonWidget(Oktagon *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/oktagon.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addParam(createParam<CKSS>(Vec(6, 65), module, Oktagon::RANGE));
        addParam(createParam<SonusKnob>(Vec(48, 60), module, Oktagon::FREQUENCY));
        addInput(createInput<PJ301MPort>(Vec(88, 66), module, Oktagon::CV_FREQ));

        addOutput(createOutput<PJ301MPort>(Vec(10, 132), module, Oktagon::WAVE0_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(48, 132), module, Oktagon::WAVE45_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(86, 132), module, Oktagon::WAVE90_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(10, 187), module, Oktagon::WAVE135_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(48, 187), module, Oktagon::WAVE180_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(86, 187), module, Oktagon::WAVE225_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(10, 242), module, Oktagon::WAVE270_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(48, 242), module, Oktagon::WAVE315_OUT));

        addChild(createLight<MediumLight<RedLight>>(Vec(58, 310), module, Oktagon::FREQ_LIGHT));
    }
};

Model *modelOktagon = createModel<Oktagon, OktagonWidget>("Oktagon");
