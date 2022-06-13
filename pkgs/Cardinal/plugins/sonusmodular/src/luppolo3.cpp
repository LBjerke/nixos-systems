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


struct Luppolo3 : Module
{
    enum ParamIds
    {
        DIRECTION,
        ERASE,
        CLEAR_M,
        CLEAR_S1,
        CLEAR_S2,
        GAIN_M,
        GAIN_S1,
        GAIN_S2,
        TRIGGER_M,
        TRIGGER_S1,
        TRIGGER_S2,
        OVERDUB_M,
        OVERDUB_S1,
        OVERDUB_S2,
        NUM_PARAMS
    };
    enum InputIds
    {
        INPUT_L,
        INPUT_R,
        TRIGGER_DIR,
        CV_TRIGGER_M,
        CV_TRIGGER_S1,
        CV_TRIGGER_S2,
        CV_OVERDUB_M,
        CV_OVERDUB_S1,
        CV_OVERDUB_S2,
        CV_ERASE,
        CV_CLEAR_M,
        CV_CLEAR_S1,
        CV_CLEAR_S2,
        NUM_INPUTS
    };
    enum OutputIds
    {
        OUTPUT_L,
        OUTPUT_R,
        OUTPUT_ML,
        OUTPUT_MR,
        OUTPUT_S1L,
        OUTPUT_S1R,
        OUTPUT_S2L,
        OUTPUT_S2R,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        REC_LIGHT_M,
        PLAY_LIGHT_M,
        REC_LIGHT_S1,
        PLAY_LIGHT_S1,
        REC_LIGHT_S2,
        PLAY_LIGHT_S2,
        NUM_LIGHTS
    };

    Luppolo3()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Luppolo3::DIRECTION, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::ERASE, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::GAIN_M, 0.0, 2.0, 1.0, "");
        configParam(Luppolo3::TRIGGER_M, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::OVERDUB_M, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::CLEAR_M, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::GAIN_S1, 0.0, 2.0, 1.0, "");
        configParam(Luppolo3::TRIGGER_S1, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::OVERDUB_S1, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::CLEAR_S1, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::GAIN_S2, 0.0, 2.0, 1.0, "");
        configParam(Luppolo3::TRIGGER_S2, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::OVERDUB_S2, 0.0, 1.0, 0.0, "");
        configParam(Luppolo3::CLEAR_S2, 0.0, 1.0, 0.0, "");
    }

    ~Luppolo3()
    {
        for (int c = 0; c < 2; c++)
        {
            master_loop[c].clear();
            slave_loop_1[c].clear();
            slave_loop_2[c].clear();
        }
    }
    void process(const ProcessArgs &args) override;

    std::deque<float> master_loop[2];
    std::deque<float> slave_loop_1[2];
    std::deque<float> slave_loop_2[2];
    bool is_recording[3] = {false, false, false};
    bool master_rec = false;
    bool overdubbing[3] = {false, false, false};
    int sample = 0;
    float trig_last_value[3][2] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
    float overdub_last_value[3][2] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
    float clear_last_value[3][2] = {{0.0, 0.0}, {0.0, 0.0}, {0.0, 0.0}};
    float erase_last_value[2] = {0.0, 0.0};
    float dir_last_value[2] = {0.0, 0.0};
    bool forward = true;
    int loop_size = 0;
};


void Luppolo3::process(const ProcessArgs &args)
{
    float in_l = inputs[INPUT_L].getVoltage();
    float in_r = inputs[INPUT_R].getVoltage();
    float out_l = 0.0;
    float out_r = 0.0;

    if (((inputs[TRIGGER_DIR].getVoltage() != 0.0) && dir_last_value[0] == 0.0) || ((params[DIRECTION].getValue() != 0.0) && (dir_last_value[1] == 0.0)))
    {
        forward = !forward;
    }

    dir_last_value[0] = inputs[TRIGGER_DIR].getVoltage();
    dir_last_value[1] = params[DIRECTION].getValue();

    // Master track
    if (((inputs[CV_TRIGGER_M].getVoltage() != trig_last_value[0][0]) && (trig_last_value[0][0] == 0.0)) || ((params[TRIGGER_M].getValue() != trig_last_value[0][1]) && (trig_last_value[0][1] == 0.0)))
    {
        if (!is_recording[0])
        {
            for (int c = 0; c < 2; c++)
            {
                master_loop[c].clear();
                slave_loop_1[c].clear();
                slave_loop_2[c].clear();
            }
            sample = 0;
            loop_size = 0;
            master_rec = false;
            overdubbing[0] = false;
        }
        else
        {
            master_rec = true;
        }

        is_recording[0] = !is_recording[0];
    }

    trig_last_value[0][0] = inputs[CV_TRIGGER_M].getVoltage();
    trig_last_value[0][1] = params[TRIGGER_M].getValue();

    if (((inputs[CV_OVERDUB_M].getVoltage() != overdub_last_value[0][0]) && (overdub_last_value[0][0] == 0.0)) || ((params[OVERDUB_M].getValue() != overdub_last_value[0][1]) && (overdub_last_value[0][1] == 0.0)))
    {
        if (!overdubbing[0] && master_rec)
        {
            overdubbing[0] = true;
        }
        else if (overdubbing && master_rec)
        {
            overdubbing[0] = false;
        }
    }

    overdub_last_value[0][0] = inputs[CV_OVERDUB_M].getVoltage();
    overdub_last_value[0][1] = params[OVERDUB_M].getValue();

    if (((inputs[CV_CLEAR_M].getVoltage() != 0.0) && clear_last_value[0][0] == 0.0) || ((params[CLEAR_M].getValue() != 0.0) && (clear_last_value[0][1] == 0.0)))
    {
        master_rec = false;
        is_recording[0] = false;
        overdubbing[0] = false;
        for (int c = 0; c < 2; c++)
        {
            master_loop[c].assign(master_loop[c].size(), 0.0);
        }
    }

    clear_last_value[0][0] = inputs[CV_CLEAR_M].getVoltage();
    clear_last_value[0][1] = params[CLEAR_M].getValue();

    if (is_recording[0])
    {
        ++loop_size;

        if (loop_size < INT_MAX)
        {
            master_loop[0].push_back(in_l);
            master_loop[1].push_back(in_r);
            slave_loop_1[0].push_back(0.0);
            slave_loop_1[1].push_back(0.0);
            slave_loop_2[0].push_back(0.0);
            slave_loop_2[1].push_back(0.0);
        }
        else
        {
            --loop_size;
            is_recording[0] = false;
            master_rec = true;
        }
    }
    else
    {
        if (!master_loop[0].empty())
        {
            if (overdubbing[0])
            {
                master_loop[0].at(sample) += in_l;
                master_loop[1].at(sample) += in_r;
            }

            out_l += master_loop[0].at(sample) * params[GAIN_M].getValue();
            out_r += master_loop[1].at(sample) * params[GAIN_M].getValue();
        }
        else
        {
            out_l += 0.0;
            out_r += 0.0;
        }
    }

    // Slave tracks
    if (master_rec)
    {
        for (int t = 1; t < 3; t++)
        {

            if (((inputs[CV_TRIGGER_M + t].getVoltage() != trig_last_value[t][0]) && (trig_last_value[t][0] == 0.0)) || ((params[TRIGGER_M + t].getValue() != trig_last_value[t][1]) && (trig_last_value[t][1] == 0.0)))
            {
                if (!is_recording[t])
                {
                    for (int c = 0; c < 2; c++)
                    {
                        t < 2 ? slave_loop_1[c].assign(slave_loop_1[c].size(), 0.0) : slave_loop_2[c].assign(slave_loop_2[c].size(), 0.0);
                    }
                    overdubbing[t] = false;
                }

                is_recording[t] = !is_recording[t];
            }

            trig_last_value[t][0] = inputs[CV_TRIGGER_M + t].getVoltage();
            trig_last_value[t][1] = params[TRIGGER_M + t].getValue();

            if (((inputs[CV_OVERDUB_M + t].getVoltage() != overdub_last_value[t][0]) && (overdub_last_value[t][0] == 0.0)) || ((params[OVERDUB_M + t].getValue() != overdub_last_value[t][1]) && (overdub_last_value[t][1] == 0.0)))
            {
                if (!overdubbing[t] && master_rec)
                {
                    overdubbing[t] = true;
                }
                else if (overdubbing && master_rec)
                {
                    overdubbing[t] = false;
                }
            }

            overdub_last_value[t][0] = inputs[CV_OVERDUB_M + t].getVoltage();
            overdub_last_value[t][1] = params[OVERDUB_M + t].getValue();


            if (((inputs[CV_CLEAR_M + t].getVoltage() != 0.0) && clear_last_value[t][0] == 0.0) || ((params[CLEAR_M].getValue() != 0.0) && (clear_last_value[t][1] == 0.0)))
            {
                is_recording[t] = false;
                overdubbing[t] = false;
                for (int c = 0; c < 2; c++)
                {
                    t < 2 ? slave_loop_1[c].assign(slave_loop_1[c].size(), 0.0) : slave_loop_2[c].assign(slave_loop_2[c].size(), 0.0);
                }
                sample = 0;
            }

            clear_last_value[t][0] = inputs[CV_CLEAR_M + t].getVoltage();
            clear_last_value[t][1] = params[CLEAR_M + t].getValue();

            if (is_recording[t])
            {
                t < 2 ? slave_loop_1[0].at(sample) = in_l : slave_loop_2[0].at(sample) = in_l;
                t < 2 ? slave_loop_1[1].at(sample) = in_r : slave_loop_2[1].at(sample) = in_r;
            }
            else
            {
                if (!master_loop[0].empty())
                {
                    if (overdubbing[t])
                    {
                        t < 2 ? slave_loop_1[0].at(sample) += in_l : slave_loop_2[0].at(sample) += in_l;
                        t < 2 ? slave_loop_1[1].at(sample) += in_r : slave_loop_2[1].at(sample) += in_r;
                    }

                    t < 2 ? out_l += slave_loop_1[0].at(sample) * params[GAIN_S1].getValue() : out_l += slave_loop_2[0].at(sample) * params[GAIN_S2].getValue();
                    t < 2 ? out_r += slave_loop_1[1].at(sample) * params[GAIN_S1].getValue() : out_r += slave_loop_2[1].at(sample) * params[GAIN_S2].getValue();
                }
                else
                {
                    out_l += 0.0;
                    out_r += 0.0;
                }
            }
        }
    }

    (is_recording[0] || overdubbing[0]) ? lights[REC_LIGHT_M].value = 1.0 : lights[REC_LIGHT_M].value = 0.0;
    (is_recording[1] || overdubbing[1]) ? lights[REC_LIGHT_S1].value = 1.0 : lights[REC_LIGHT_S1].value = 0.0;
    (is_recording[2] || overdubbing[2]) ? lights[REC_LIGHT_S2].value = 1.0 : lights[REC_LIGHT_S2].value = 0.0;
    master_rec ? lights[PLAY_LIGHT_M].value = 1.0 : lights[PLAY_LIGHT_M].value = 0.0;
    master_rec ? lights[PLAY_LIGHT_S1].value = 1.0 : lights[PLAY_LIGHT_S1].value = 0.0;
    master_rec ? lights[PLAY_LIGHT_S2].value = 1.0 : lights[PLAY_LIGHT_S2].value = 0.0;

    if (is_recording[0] || is_recording[1] || is_recording[2])
    {
        out_l += in_l;
        out_r += in_r;
    }

    outputs[OUTPUT_L].setVoltage(out_l);
    outputs[OUTPUT_R].setVoltage(out_r);

    if (master_rec)
    {
        outputs[OUTPUT_ML].setVoltage(master_loop[0].at(sample) * params[GAIN_M].getValue());
        outputs[OUTPUT_MR].setVoltage(master_loop[1].at(sample) * params[GAIN_M].getValue());
        outputs[OUTPUT_S1L].setVoltage(slave_loop_1[0].at(sample) * params[GAIN_S1].getValue());
        outputs[OUTPUT_S1R].setVoltage(slave_loop_1[1].at(sample) * params[GAIN_S1].getValue());
        outputs[OUTPUT_S2L].setVoltage(slave_loop_2[0].at(sample) * params[GAIN_S2].getValue());
        outputs[OUTPUT_S2R].setVoltage(slave_loop_2[1].at(sample) * params[GAIN_S2].getValue());
    }

    if (!is_recording[0])
    {
        if (forward)
        {
            if (++sample >= (int)master_loop[0].size())
            {
                sample = 0;
            }
        }
        else
        {
            if (--sample < 0)
            {
                sample = master_loop[0].size() - 1;
            }
        }
    }

    if (((inputs[CV_ERASE].getVoltage() != 0.0) && erase_last_value[0] == 0.0) || ((params[ERASE].getValue() != 0.0) && (erase_last_value[1] == 0.0)))
    {
        master_rec = false;
        for (int t = 0; t < 3; t++)
        {
            is_recording[t] = false;
            overdubbing[t] = false;
        }
        for (int c = 0; c < 2; c++)
        {
            master_loop[c].clear();
            slave_loop_1[c].clear();
            slave_loop_2[c].clear();
        }
        sample = 0;
        loop_size = 0;
    }

    erase_last_value[0] = inputs[CV_ERASE].getVoltage();
    erase_last_value[1] = params[ERASE].getValue();
}

struct Luppolo3Widget : ModuleWidget
{
    Luppolo3Widget(Luppolo3 *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/luppolo3.svg")));

        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addInput(createInput<PJ301MPort>(Vec(14, 92), module, Luppolo3::INPUT_L));
        addInput(createInput<PJ301MPort>(Vec(52, 92), module, Luppolo3::INPUT_R));
        addInput(createInput<PJ301MPort>(Vec(14, 215), module, Luppolo3::TRIGGER_DIR));
        addParam(createParam<CKD6>(Vec(50, 213), module, Luppolo3::DIRECTION));
        addInput(createInput<PJ301MPort>(Vec(14,272), module, Luppolo3::CV_ERASE));
        addParam(createParam<CKD6>(Vec(50,270), module, Luppolo3::ERASE));

        addParam(createParam<SonusKnob>(Vec(117, 85), module, Luppolo3::GAIN_M));
        addInput(createInput<PJ301MPort>(Vec(104, 155), module, Luppolo3::CV_TRIGGER_M));
        addParam(createParam<CKD6>(Vec(140, 153), module, Luppolo3::TRIGGER_M));
        addInput(createInput<PJ301MPort>(Vec(104, 215), module, Luppolo3::CV_OVERDUB_M));
        addParam(createParam<CKD6>(Vec(140, 213), module, Luppolo3::OVERDUB_M));
        addInput(createInput<PJ301MPort>(Vec(104,272), module, Luppolo3::CV_CLEAR_M));
        addParam(createParam<CKD6>(Vec(140,270), module, Luppolo3::CLEAR_M));

        addParam(createParam<SonusKnob>(Vec(207, 85), module, Luppolo3::GAIN_S1));
        addInput(createInput<PJ301MPort>(Vec(194, 155), module, Luppolo3::CV_TRIGGER_S1));
        addParam(createParam<CKD6>(Vec(230, 153), module, Luppolo3::TRIGGER_S1));
        addInput(createInput<PJ301MPort>(Vec(194, 215), module, Luppolo3::CV_OVERDUB_S1));
        addParam(createParam<CKD6>(Vec(230, 213), module, Luppolo3::OVERDUB_S1));
        addInput(createInput<PJ301MPort>(Vec(194, 272), module, Luppolo3::CV_CLEAR_S1));
        addParam(createParam<CKD6>(Vec(230,270), module, Luppolo3::CLEAR_S1));

        addParam(createParam<SonusKnob>(Vec(297, 85), module, Luppolo3::GAIN_S2));
        addInput(createInput<PJ301MPort>(Vec(284, 155), module, Luppolo3::CV_TRIGGER_S2));
        addParam(createParam<CKD6>(Vec(320, 153), module, Luppolo3::TRIGGER_S2));
        addInput(createInput<PJ301MPort>(Vec(284, 215), module, Luppolo3::CV_OVERDUB_S2));
        addParam(createParam<CKD6>(Vec(320, 213), module, Luppolo3::OVERDUB_S2));
        addInput(createInput<PJ301MPort>(Vec(284,272), module, Luppolo3::CV_CLEAR_S2));
        addParam(createParam<CKD6>(Vec(320,270), module, Luppolo3::CLEAR_S2));

        addOutput(createOutput<PJ301MPort>(Vec(374, 92), module, Luppolo3::OUTPUT_L));
        addOutput(createOutput<PJ301MPort>(Vec(412, 92), module, Luppolo3::OUTPUT_R));
        addOutput(createOutput<PJ301MPort>(Vec(374, 175), module, Luppolo3::OUTPUT_ML));
        addOutput(createOutput<PJ301MPort>(Vec(412, 175), module, Luppolo3::OUTPUT_MR));
        addOutput(createOutput<PJ301MPort>(Vec(374, 230), module, Luppolo3::OUTPUT_S1L));
        addOutput(createOutput<PJ301MPort>(Vec(412, 230), module, Luppolo3::OUTPUT_S1R));
        addOutput(createOutput<PJ301MPort>(Vec(374, 286), module, Luppolo3::OUTPUT_S2L));
        addOutput(createOutput<PJ301MPort>(Vec(412, 286), module, Luppolo3::OUTPUT_S2R));

        addChild(createLight<MediumLight<RedLight>>(Vec(113, 65), module, Luppolo3::REC_LIGHT_M));
        addChild(createLight<MediumLight<GreenLight>>(Vec(148, 65), module, Luppolo3::PLAY_LIGHT_M));
        addChild(createLight<MediumLight<RedLight>>(Vec(203, 65), module, Luppolo3::REC_LIGHT_S1));
        addChild(createLight<MediumLight<GreenLight>>(Vec(238, 65), module, Luppolo3::PLAY_LIGHT_S1));
        addChild(createLight<MediumLight<RedLight>>(Vec(293, 65), module, Luppolo3::REC_LIGHT_S2));
        addChild(createLight<MediumLight<GreenLight>>(Vec(328, 65), module, Luppolo3::PLAY_LIGHT_S2));
    }
};

Model *modelLuppolo3 = createModel<Luppolo3, Luppolo3Widget>("Luppolo3");
