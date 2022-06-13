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


struct Pusher : Module
{
    enum ParamIds
    {
        PUSH_1,
        CV_1,
        PUSH_2,
        CV_2,
        PUSH_3,
        CV_3,
        PUSH_4,
        CV_4,
        NUM_PARAMS
    };
    enum InputIds
    {
        NUM_INPUTS
    };
    enum OutputIds
    {
        OUT_1,
        OUT_2,
        OUT_3,
        OUT_4,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        NUM_LIGHTS
    };

    Pusher()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Pusher::CV_1, -5.0, 5.0, 5.0, "");
        configParam(Pusher::CV_2, -5.0, 5.0, 5.0, "");
        configParam(Pusher::CV_3, -5.0, 5.0, 5.0, "");
        configParam(Pusher::CV_4, -5.0, 5.0, 5.0, "");
        configParam(Pusher::PUSH_1, 0.0, 1.0, 0.0, "");
        configParam(Pusher::PUSH_2, 0.0, 1.0, 0.0, "");
        configParam(Pusher::PUSH_3, 0.0, 1.0, 0.0, "");
        configParam(Pusher::PUSH_4, 0.0, 1.0, 0.0, "");

    }
    void process(const ProcessArgs &args) override;
};


void Pusher::process(const ProcessArgs &args)
{
    outputs[OUT_1].setVoltage(params[CV_1].getValue() * params[PUSH_1].getValue());
    outputs[OUT_2].setVoltage(params[CV_2].getValue() * params[PUSH_2].getValue());
    outputs[OUT_3].setVoltage(params[CV_3].getValue() * params[PUSH_3].getValue());
    outputs[OUT_4].setVoltage(params[CV_4].getValue() * params[PUSH_4].getValue());
}

struct PusherWidget : ModuleWidget
{
    PusherWidget(Pusher *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/pusher.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addOutput(createOutput<PJ301MPort>(Vec(18, 157), module, Pusher::OUT_1));
        addOutput(createOutput<PJ301MPort>(Vec(18, 292), module, Pusher::OUT_2));
        addOutput(createOutput<PJ301MPort>(Vec(78, 157), module, Pusher::OUT_3));
        addOutput(createOutput<PJ301MPort>(Vec(78, 292), module, Pusher::OUT_4));

        addParam(createParam<SonusKnob>(Vec(12, 99), module, Pusher::CV_1));
        addParam(createParam<SonusKnob>(Vec(12, 235), module, Pusher::CV_2));
        addParam(createParam<SonusKnob>(Vec(72, 99), module, Pusher::CV_3));
        addParam(createParam<SonusKnob>(Vec(72, 235), module, Pusher::CV_4));

        addParam(createParam<CKD6>(Vec(16,64), module, Pusher::PUSH_1));
        addParam(createParam<CKD6>(Vec(16,199), module, Pusher::PUSH_2));
        addParam(createParam<CKD6>(Vec(76,64), module, Pusher::PUSH_3));
        addParam(createParam<CKD6>(Vec(76,199), module, Pusher::PUSH_4));
    }
};

Model *modelPusher = createModel<Pusher, PusherWidget>("Pusher");
