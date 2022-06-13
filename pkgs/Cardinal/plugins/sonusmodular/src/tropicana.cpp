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
#include "tropadd.hpp"


struct Tropicana : Module
{
    enum ParamIds
    {
        OCTAVE,
        OSC1_BEATS,
        OSC1_VCA,
        OSC2_BEATS,
        OSC2_VCA,
        OSC3_BEATS,
        OSC3_VCA,
        OSC4_BEATS,
        OSC4_VCA,
        OSC5_BEATS,
        OSC5_VCA,
        OFFSET,
        HARM_MODE,
        NUM_PARAMS
    };
    enum InputIds
    {
        CV_PITCH,
        CV_OSC1_BEATS,
        CV_OSC1_VCA,
        CV_OSC2_BEATS,
        CV_OSC2_VCA,
        CV_OSC3_BEATS,
        CV_OSC3_VCA,
        CV_OSC4_BEATS,
        CV_OSC4_VCA,
        CV_OSC5_BEATS,
        CV_OSC5_VCA,
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

    Tropicana()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Tropicana::OCTAVE, -3.0, 3.0, 0.0, "");
        configParam(Tropicana::OSC1_BEATS, -10.0, 10.0, 0.0, "");
        configParam(Tropicana::OSC1_VCA, -1.0, 1.0, 0.0, "");
        configParam(Tropicana::OSC2_BEATS, -10.0, 10.0, 0.0, "");
        configParam(Tropicana::OSC2_VCA, -1.0, 1.0, 0.0, "");
        configParam(Tropicana::OSC3_BEATS, -10.0, 10.0, 0.0, "");
        configParam(Tropicana::OSC3_VCA, -1.0, 1.0, 0.0, "");
        configParam(Tropicana::OSC4_BEATS, -10.0, 10.0, 0.0, "");
        configParam(Tropicana::OSC4_VCA, -1.0, 1.0, 0.0, "");
        configParam(Tropicana::OSC5_BEATS, -10.0, 10.0, 0.0, "");
        configParam(Tropicana::OSC5_VCA, -1.0, 1.0, 0.0, "");

        configParam(Tropicana::OFFSET, -2.0, 2.0, 0.0, "");
        configParam(Tropicana::HARM_MODE, 0.0, 2.0, 2.0, "");
    }

    void process(const ProcessArgs &args) override;

    TropAdd<float> additive;
};


void Tropicana::process(const ProcessArgs &args)
{
    float pitch = params[OCTAVE].getValue();
    pitch += inputs[CV_PITCH].getVoltage();
    pitch = clamp(pitch, -4.0f, 4.0f);

    float frequency = 261.626f * powf(2.0f, pitch);

    std::vector<float> beats(5, 0.0f);

    beats[0] = params[OSC1_BEATS].getValue() + inputs[CV_OSC1_BEATS].getVoltage();
    beats[1] = params[OSC2_BEATS].getValue() + inputs[CV_OSC2_BEATS].getVoltage();
    beats[2] = params[OSC3_BEATS].getValue() + inputs[CV_OSC3_BEATS].getVoltage();
    beats[3] = params[OSC4_BEATS].getValue() + inputs[CV_OSC4_BEATS].getVoltage();
    beats[4] = params[OSC5_BEATS].getValue() + inputs[CV_OSC5_BEATS].getVoltage();

    additive.set_mode((HarmonicModes)params[HARM_MODE].getValue());

    additive.set_frequency(frequency, beats);

    for (int o = 0; o < 5; o++)
    {
        float osc_vca = clamp((float)(params[OSC1_VCA + (o * 2)].getValue() + (inputs[CV_OSC1_VCA + (o * 2)].getVoltage() * 0.2)), -1.0f, 1.0f);
        additive.set_vca(osc_vca, o);
    }

    additive.set_offset(params[OFFSET].getValue());

    outputs[OUTPUT].setVoltage(additive.run() * 5.0f);
}

struct TropicanaWidget : ModuleWidget
{
    TropicanaWidget(Tropicana *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/tropicana.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addParam(createParam<SonusKnob>(Vec(20, 64), module, Tropicana::OCTAVE));
        addInput(createInput<PJ301MPort>(Vec(25.5, 137), module, Tropicana::CV_PITCH));

        addOutput(createOutput<PJ301MPort>(Vec(25.5, 277), module, Tropicana::OUTPUT));

        for (unsigned int a = 0; a < 5; a++)
        {
            addParam(createParam<SonusKnob>(Vec(150 + (50 * a), 64), module, Tropicana::OSC1_BEATS + (a * 2)));
            addInput(createInput<PJ301MPort>(Vec(155.5 + (50 * a), 102), module, Tropicana::CV_OSC1_BEATS + (a * 2)));
        }

        for (unsigned int a = 0; a < 5; a++)
        {
            addParam(createParam<SonusKnob>(Vec(150 + (50 * a), 134), module, Tropicana::OSC1_VCA + (a * 2)));
            addInput(createInput<PJ301MPort>(Vec(155.5 + (50 * a), 172), module, Tropicana::CV_OSC1_VCA + (a * 2)));
        }

        addParam(createParam<SonusKnob>(Vec(150, 264), module, Tropicana::OFFSET));
        addParam(createParam<NKK>(Vec(305, 260.11), module, Tropicana::HARM_MODE));
    }
};

Model *modelTropicana = createModel<Tropicana, TropicanaWidget>("Tropicana");
