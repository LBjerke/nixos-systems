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


struct Fraction : Module
{
    enum ParamIds
    {
        OCTAVE,
        POWER_1,
        POWER_2,
        NUM_PARAMS
    };
    enum InputIds
    {
        PITCH,
        CV_POWER_1,
        CV_POWER_2,
        NUM_INPUTS
    };
    enum OutputIds
    {
        WAVE1_OUT,
        WAVE2_OUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Fraction()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Fraction::OCTAVE, -3.0, 3.0, 0.0, "");
        configParam(Fraction::POWER_1, 0.01, 2.0, 1.0, "");
        configParam(Fraction::POWER_2, -2.0, -0.01, -1.0, "");
    }

    void process(const ProcessArgs &args) override;

    float step_count = 0.0;
};


void Fraction::process(const ProcessArgs &args)
{
    float pitch = params[OCTAVE].getValue();
    pitch += inputs[PITCH].getVoltage();
    pitch = clamp(pitch, -4.0, 4.0);
    float freq = 261.626 * powf(2.0, pitch);

    float power_1 = params[POWER_1].getValue();
    power_1 += inputs[CV_POWER_1].getVoltage() * 0.2;
    float power_2 = params[POWER_2].getValue();
    power_2 += inputs[CV_POWER_2].getVoltage() * (-0.2);

    float steps = floor(args.sampleRate / freq);

    if (step_count >= steps)
    {
        step_count = 0;
    }

    float out1 = ((powf(step_count, power_1)/(powf(step_count, power_1) + 1.0)) * 2.0) - 1.0;
    float out2 = ((powf(step_count, power_2)/(powf(step_count, power_2) + 1.0)) * 2.0) - 0.5;

    out1 = clamp(out1 * 5.0f, -5.0f, 5.0f);
    out2 = clamp(out2 * 10.0f, -5.0f, 5.0f);

    ++step_count;

    outputs[WAVE1_OUT].setVoltage(out1);
    outputs[WAVE2_OUT].setVoltage(out2);
}

struct FractionWidget : ModuleWidget
{
    FractionWidget(Fraction *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/fraction.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Fraction::PITCH));
        addParam(createParam<SonusSnapKnob>(Vec(46, 61), module, Fraction::OCTAVE));
        addOutput(createOutput<PJ301MPort>(Vec(14, 132), module, Fraction::WAVE1_OUT));
        addOutput(createOutput<PJ301MPort>(Vec(52, 132), module, Fraction::WAVE2_OUT));
        addInput(createInput<PJ301MPort>(Vec(14, 197), module, Fraction::CV_POWER_1));
        addInput(createInput<PJ301MPort>(Vec(52, 197), module, Fraction::CV_POWER_2));
        addParam(createParam<SonusKnob>(Vec(8, 256), module, Fraction::POWER_1));
        addParam(createParam<SonusKnob>(Vec(46, 256), module, Fraction::POWER_2));
    }
};

Model *modelFraction = createModel<Fraction, FractionWidget>("Fraction");
