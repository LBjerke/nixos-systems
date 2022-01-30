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


struct Luppolo : Module
{
    enum ParamIds
    {
        CLEAR,
        NUM_PARAMS
    };
    enum InputIds
    {
        INPUT,
        TRIGGER,
        OVERDUB,
        CV_CLEAR,
        NUM_INPUTS
    };
    enum OutputIds
    {
        OUTPUT,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        REC_LIGHT,
        PLAY_LIGHT,
        NUM_LIGHTS
    };

    Luppolo()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Luppolo::CLEAR, 0.0, 1.0, 0.0, "");
    }
    void process(const ProcessArgs &args) override;

    std::vector<float> loop;
    bool is_recording = false;
    bool master_rec = false;
    bool overdubbing = false;
    unsigned int sample = 0;
    float trig_last_value = 0.0;
    float overdub_last_value = 0.0;
};


void Luppolo::process(const ProcessArgs &args)
{
    float in = inputs[INPUT].getVoltage();
    float out = 0.0;

    if ((inputs[TRIGGER].getVoltage() != trig_last_value) && (trig_last_value == 0.0))
    {
        if (!is_recording)
        {
            loop.clear();
            sample = 0;
            master_rec = false;
            overdubbing = false;
        }
        else
        {
            master_rec = true;
        }

        is_recording = !is_recording;
    }

    trig_last_value = inputs[TRIGGER].getVoltage();

    if ((inputs[OVERDUB].getVoltage() != overdub_last_value) && (overdub_last_value == 0.0))
    {
        if (!overdubbing && master_rec)
        {
            overdubbing = true;
        }
        else if (overdubbing && master_rec)
        {
            overdubbing = false;
        }
    }

    overdub_last_value = inputs[OVERDUB].getVoltage();

    if ((params[CLEAR].getValue() != 0.0) || (inputs[CV_CLEAR].getVoltage() != 0.0))
    {
        master_rec = false;
        is_recording = false;
        overdubbing = false;
        loop.clear();
        sample = 0;
    }

    if (is_recording)
    {
        out = in;
        loop.push_back(in);
    }
    else
    {
        if (!loop.empty())
        {
            if (overdubbing)
            {
                loop.at(sample) += in;
            }

            out = loop.at(sample);
        }
        else
        {
            out = 0.0;
        }

        if (++sample >= loop.size())
        {
            sample = 0;
        }
    }

    outputs[OUTPUT].setVoltage(out);

    if (is_recording || overdubbing)
    {
        lights[REC_LIGHT].value = 1.0;
    }
    else
    {
        lights[REC_LIGHT].value = 0.0;
    }

    if (master_rec)
    {
        lights[PLAY_LIGHT].value = 1.0;
    }
    else
    {
        lights[PLAY_LIGHT].value = 0.0;
    }
}

struct LuppoloWidget : ModuleWidget
{
    LuppoloWidget(Luppolo *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/luppolo.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Luppolo::INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(52, 67), module, Luppolo::OUTPUT));
        addInput(createInput<PJ301MPort>(Vec(33, 155), module, Luppolo::TRIGGER));
        addInput(createInput<PJ301MPort>(Vec(33, 215), module, Luppolo::OVERDUB));

        addInput(createInput<PJ301MPort>(Vec(14,272), module, Luppolo::CV_CLEAR));
        addParam(createParam<CKD6>(Vec(50,270), module, Luppolo::CLEAR));

        addChild(createLight<MediumLight<RedLight>>(Vec(22, 127), module, Luppolo::REC_LIGHT));
        addChild(createLight<MediumLight<GreenLight>>(Vec(61, 127), module, Luppolo::PLAY_LIGHT));
    }
};

Model *modelLuppolo = createModel<Luppolo, LuppoloWidget>("Luppolo");
