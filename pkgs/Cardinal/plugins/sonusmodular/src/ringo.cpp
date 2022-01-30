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


struct Ringo : Module
{
    enum ParamIds
    {
        SOURCE,
        FREQUENCY,
        SHAPE,
        NUM_PARAMS
    };
    enum InputIds
    {
        CARRIER,
        MODULATOR,
        CV_FREQ,
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

    Ringo()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Ringo::SOURCE, 0.0, 1.0, 0.0, "");
        configParam(Ringo::SHAPE, 0.0, 1.0, 0.0, "");
        configParam(Ringo::FREQUENCY, -2.0, 2.0, 0.0, "");

    }

    void process(const ProcessArgs &args) override;

    float phase = 0.0;
    bool audio_modulated = false;
};


void Ringo::process(const ProcessArgs &args)
{
    float freq = 0.0;
    float sine_out = 0.0;
    float saw_out = 0.0;
    float shape = params[SHAPE].getValue();
    float carrier = inputs[CARRIER].getVoltage() / 5.0;
    float modulator = inputs[MODULATOR].getVoltage() / 5.0;

    if (params[SOURCE].getValue() == 0.0)
    {
        audio_modulated = false;
    }
    else
    {
        audio_modulated = true;
    }

    float pitch = params[FREQUENCY].getValue();
    pitch += inputs[CV_FREQ].getVoltage();
    pitch = clamp(pitch, -4.0, 4.0);
    freq = 440.0 * powf(2.0, pitch);

    phase += freq / args.sampleRate;
    if (phase >= 1.0)
    {
        phase -= 1.0;
    }

    sine_out = sinf(2.0 * M_PI * phase);
    saw_out = 2.0 * (phase - 0.5);

    if (audio_modulated)
    {
        outputs[OUTPUT].setVoltage(carrier * modulator * 5.0);
    }
    else
    {
        outputs[OUTPUT].setVoltage(carrier * (((1.0 - shape) * sine_out) + (shape * saw_out)) * 5.0);
    }
}

struct RingoWidget : ModuleWidget
{
    RingoWidget(Ringo *module)
    {
        (APP->window->loadSvg(asset::plugin(pluginInstance, "res/ringo.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Ringo::MODULATOR));
        addInput(createInput<PJ301MPort>(Vec(52, 67), module, Ringo::CARRIER));
        addOutput(createOutput<PJ301MPort>(Vec(33, 132), module, Ringo::OUTPUT));

        addParam(createParam<CKSS>(Vec(28, 197), module, Ringo::SOURCE));
        addParam(createParam<SonusKnob>(Vec(27, 243), module, Ringo::SHAPE));
        addParam(createParam<SonusKnob>(Vec(48, 293), module, Ringo::FREQUENCY));
        addInput(createInput<PJ301MPort>(Vec(14, 300), module, Ringo::CV_FREQ));
    }
};

Model *modelRingo = createModel<Ringo, RingoWidget>("Ringo");
