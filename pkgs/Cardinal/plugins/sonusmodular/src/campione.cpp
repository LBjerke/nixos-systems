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


struct Campione : Module
{
    enum ParamIds
    {
        RECORD,
        PLAY,
        SPEED,
        DIRECTION,
        CLEAR,
        START,
        END,
        LOOP,
        ONE_SHOT,
        NUM_PARAMS
    };
    enum InputIds
    {
        INPUT,
        CV_RECORD,
        CV_PLAY,
        CV_SPEED,
        CV_DIRECTION,
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

    Campione()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);
        configParam(Campione::RECORD, 0.0, 1.0, 0.0, "");
        configParam(Campione::PLAY, 0.0, 1.0, 0.0, "");
        configParam(Campione::CLEAR, 0.0, 1.0, 0.0, "");
        configParam(Campione::DIRECTION, 0.0, 1.0, 0.0, "");
        configParam(Campione::SPEED, 0.0, 2.0, 1.0, "");
        configParam(Campione::ONE_SHOT, 0.0, 1.0, 0.0, "");
        configParam(Campione::LOOP, 0.0, 1.0, 0.0, "");
        configParam(Campione::START, 0.0, 1.0, 0.0, "");
        configParam(Campione::END, 0.0, 1.0, 1.0, "");

    }
    void process(const ProcessArgs &args) override;

    std::deque<float> sample;
    bool loop = false;
    bool forward = true;
    bool recording = false;
    bool one_shot = true;
    bool playing = false;
    float pos = 0.0;
    unsigned int count = 0;
    float play_last_value[2] = {0.0, 0.0};
    float rec_last_value[2] = {0.0, 0.0};
    float clear_last_value[2] = {0.0, 0.0};
    float dir_last_value[2] = {0.0, 0.0};
    float increase = 0.0;
    int start = 0;
    int end = 0;
};


void Campione::process(const ProcessArgs &args)
{
    float in = inputs[INPUT].getVoltage();
    float out = 0.0;

    if (!sample.empty())
    {
        start = (unsigned int)(floorf(params[START].getValue() * sample.size()));
        end = (unsigned int)(ceilf(params[END].getValue() * sample.size()));

        if (start == (int)sample.size())
        {
            --start;

            if (start < 0)
            {
                start = 0;
            }
        }

        if (end < start)
        {
            end = start;
        }
    }
    else
    {
        start = 0;
        end = 0;
    }

    params[ONE_SHOT].getValue() == 0.0 ? one_shot = false : one_shot = true;
    params[LOOP].getValue() == 0.0 ? loop = false : loop = true;

    increase = (((inputs[CV_SPEED].getVoltage() + 5.0) / 5.0) + params[SPEED].getValue()) / 2.0;

    if (((inputs[CV_RECORD].getVoltage() != rec_last_value[0]) && (rec_last_value[0] == 0.0)) || ((params[RECORD].getValue() != rec_last_value[1]) && (rec_last_value[1] == 0.0)))
    {
        if (!recording)
        {
            sample.clear();
            pos = 0;
        }

        recording = !recording;
    }

    rec_last_value[0] = inputs[CV_RECORD].getVoltage();
    rec_last_value[1] = params[RECORD].getValue();

    if (((inputs[CV_DIRECTION].getVoltage() != dir_last_value[0]) && (dir_last_value[0] == 0.0)) || ((params[DIRECTION].getValue() != dir_last_value[1]) && (dir_last_value[1] == 0.0)))
    {
        forward = !forward;
    }

    dir_last_value[0] = inputs[CV_DIRECTION].getVoltage();
    dir_last_value[1] = params[DIRECTION].getValue();

    if (one_shot)
    {
        if (((inputs[CV_PLAY].getVoltage() != play_last_value[0]) && (play_last_value[0] == 0.0)) || ((params[PLAY].getValue() != play_last_value[1]) && (play_last_value[1] == 0.0)))
        {
            playing = !playing;
            forward ? pos = (float)start : pos = (float)end - 1.0;
            count = 0;
        }
    }
    else
    {
        if ((inputs[CV_PLAY].getVoltage() != 0.0) || (params[PLAY].getValue() != 0.0))
        {
            playing = true;
        }
        else
        {
            playing = false;
            forward ? pos = (float)start : pos = (float)end - 1.0;
            count = 0;
        }
    }

    play_last_value[0] = inputs[CV_PLAY].getVoltage();
    play_last_value[1] = params[PLAY].getValue();

    if (count > 0 && !loop)
    {
        playing = false;
        forward ? pos = (float)start : pos = (float)end - 1.0;
    }

    if (recording)
    {
        out = in;
        sample.push_back(in);
    }
    else
    {
        if (!sample.empty() && playing)
        {
            out = sample.at((int)floorf(pos));
        }
        else
        {
            out = 0.0;
        }

        if (forward)
        {
            pos += increase;
            if (pos >= (float)end)
            {
                ++count;
                pos = (float)start;
            }
        }
        else
        {
            pos -= increase;
            if (pos < (float)start)
            {
                ++count;
                pos = (float)end - 1.0;
            }
        }
    }

    outputs[OUTPUT].setVoltage(out);

    if (((inputs[CV_CLEAR].getVoltage() != clear_last_value[0]) && (clear_last_value[0] == 0.0)) || ((params[CLEAR].getValue() != clear_last_value[1]) && (clear_last_value[1] == 0.0)))
    {
        sample.clear();
        pos = 0.0;
    }

    clear_last_value[0] = inputs[CV_CLEAR].getVoltage();
    clear_last_value[1] = params[CLEAR].getValue();

    if (recording)
    {
        lights[REC_LIGHT].value = 1.0;
    }
    else
    {
        lights[REC_LIGHT].value = 0.0;
    }

    if (playing && !recording)
    {
        lights[PLAY_LIGHT].value = 1.0;
    }
    else
    {
        lights[PLAY_LIGHT].value = 0.0;
    }
}

struct CampioneWidget : ModuleWidget
{
    CampioneWidget(Campione *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/campione.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 67), module, Campione::INPUT));
        addOutput(createOutput<PJ301MPort>(Vec(52, 67), module, Campione::OUTPUT));

        addInput(createInput<PJ301MPort>(Vec(14, 152), module, Campione::CV_RECORD));
        addParam(createParam<CKD6>(Vec(50,150), module, Campione::RECORD));

        addInput(createInput<PJ301MPort>(Vec(14, 212), module, Campione::CV_PLAY));
        addParam(createParam<CKD6>(Vec(50,210), module, Campione::PLAY));

        addInput(createInput<PJ301MPort>(Vec(14, 272), module, Campione::CV_CLEAR));
        addParam(createParam<CKD6>(Vec(50,270), module, Campione::CLEAR));

        addInput(createInput<PJ301MPort>(Vec(104, 152), module, Campione::CV_DIRECTION));
        addParam(createParam<CKD6>(Vec(140,150), module, Campione::DIRECTION));

        addInput(createInput<PJ301MPort>(Vec(104, 212), module, Campione::CV_SPEED));
        addParam(createParam<SonusKnob>(Vec(140, 206), module, Campione::SPEED));

        addParam(createParam<CKSS>(Vec(153, 53), module, Campione::ONE_SHOT));
        addParam(createParam<CKSS>(Vec(153, 85), module, Campione::LOOP));

        addParam(createParam<SonusKnob>(Vec(94, 266), module, Campione::START));
        addParam(createParam<SonusKnob>(Vec(140, 266), module, Campione::END));

        addChild(createLight<MediumLight<RedLight>>(Vec(71, 127), module, Campione::REC_LIGHT));
        addChild(createLight<MediumLight<GreenLight>>(Vec(104, 127), module, Campione::PLAY_LIGHT));
    }
};

Model *modelCampione = createModel<Campione, CampioneWidget>("Campione");
