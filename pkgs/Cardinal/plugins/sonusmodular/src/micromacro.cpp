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


#define TWO_1_12 1.059463094359295
#define TWO_1_24 1.029302236643492
#define TWO_1_72 1.009673533228511


enum Intervals
{
    CHROMATIC,
    QUARTER_TONES,
    TWELFTH_TONES
};

typedef struct
{
    bool gate;
    float pitch;
} step_info_t;

struct Micromacro : Module
{
    enum ParamIds
    {
        BPM,
        STEPS,
        BASE_OCTAVE,
        BASE_NOTE,
        TUNE,
        INTERVALS,
        CLOCK_SOURCE,
        PLAY,
        RESET,
        PITCH_1_1,
        GATE_1_1,
        PITCH_2_1,
        GATE_2_1,
        PITCH_3_1,
        GATE_3_1,
        PITCH_4_1,
        GATE_4_1,
        PITCH_5_1,
        GATE_5_1,
        PITCH_6_1,
        GATE_6_1,
        PITCH_7_1,
        GATE_7_1,
        PITCH_8_1,
        GATE_8_1,
        PITCH_9_1,
        GATE_9_1,
        PITCH_10_1,
        GATE_10_1,
        PITCH_11_1,
        GATE_11_1,
        PITCH_12_1,
        GATE_12_1,
        PITCH_13_1,
        GATE_13_1,
        PITCH_14_1,
        GATE_14_1,
        PITCH_15_1,
        GATE_15_1,
        PITCH_16_1,
        GATE_16_1,
        PITCH_1_2,
        GATE_1_2,
        PITCH_2_2,
        GATE_2_2,
        PITCH_3_2,
        GATE_3_2,
        PITCH_4_2,
        GATE_4_2,
        PITCH_5_2,
        GATE_5_2,
        PITCH_6_2,
        GATE_6_2,
        PITCH_7_2,
        GATE_7_2,
        PITCH_8_2,
        GATE_8_2,
        PITCH_9_2,
        GATE_9_2,
        PITCH_10_2,
        GATE_10_2,
        PITCH_11_2,
        GATE_11_2,
        PITCH_12_2,
        GATE_12_2,
        PITCH_13_2,
        GATE_13_2,
        PITCH_14_2,
        GATE_14_2,
        PITCH_15_2,
        GATE_15_2,
        PITCH_16_2,
        GATE_16_2,
        PITCH_1_3,
        GATE_1_3,
        PITCH_2_3,
        GATE_2_3,
        PITCH_3_3,
        GATE_3_3,
        PITCH_4_3,
        GATE_4_3,
        PITCH_5_3,
        GATE_5_3,
        PITCH_6_3,
        GATE_6_3,
        PITCH_7_3,
        GATE_7_3,
        PITCH_8_3,
        GATE_8_3,
        PITCH_9_3,
        GATE_9_3,
        PITCH_10_3,
        GATE_10_3,
        PITCH_11_3,
        GATE_11_3,
        PITCH_12_3,
        GATE_12_3,
        PITCH_13_3,
        GATE_13_3,
        PITCH_14_3,
        GATE_14_3,
        PITCH_15_3,
        GATE_15_3,
        PITCH_16_3,
        GATE_16_3,
        NUM_PARAMS
    };
    enum InputIds
    {
        EXT_CLOCK,
        CV_RESET,
        NUM_INPUTS
    };
    enum OutputIds
    {
        GATE_OUT_1,
        PITCH_OUT_1,
        GATE_OUT_2,
        PITCH_OUT_2,
        GATE_OUT_3,
        PITCH_OUT_3,
        NUM_OUTPUTS
    };
    enum LightIds
    {
        PLAY_LED,
        STEP_LED_1,
        STEP_LED_2,
        STEP_LED_3,
        STEP_LED_4,
        STEP_LED_5,
        STEP_LED_6,
        STEP_LED_7,
        STEP_LED_8,
        STEP_LED_9,
        STEP_LED_10,
        STEP_LED_11,
        STEP_LED_12,
        STEP_LED_13,
        STEP_LED_14,
        STEP_LED_15,
        STEP_LED_16,
        NUM_LIGHTS
    };

    Micromacro()
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configParam(Micromacro::BPM, 20.0, 220.0, 120.0, "");
        configParam(Micromacro::STEPS, 1.0, 16.0, 16.0, "");
        configParam(Micromacro::BASE_OCTAVE, -3.0, 3.0, 0, "");
        configParam(Micromacro::BASE_NOTE, 0.0, 11.0, 0.0, "");
        configParam(Micromacro::TUNE, 432.0, 448.0, 440.0, "");
        configParam(Micromacro::INTERVALS, 0.0, 2.0, 0.0, "");
        configParam(Micromacro::CLOCK_SOURCE, 0.0, 1.0, 0.0, "");
        configParam(Micromacro::RESET, 0.0, 1.0, 0.0, "");
        configParam(Micromacro::PLAY, 0.0, 1.0, 1.0, "");
        for (unsigned int s = 0; s < 16; s++)
        {
            configParam(Micromacro::PITCH_1_1 + (s * 2), 0.0, 17.0, 0.0, "");
            configParam(Micromacro::GATE_1_1 + (s * 2), 0.0, 1.0, 0.0, "");
            configParam(Micromacro::PITCH_1_2 + (s * 2), 0.0, 17.0, 0.0, "");
            configParam(Micromacro::GATE_1_2 + (s * 2), 0.0, 1.0, 0.0, "");
            configParam(Micromacro::PITCH_1_3 + (s * 2), 0.0, 17.0, 0.0, "");
            configParam(Micromacro::GATE_1_3 + (s * 2), 0.0, 1.0, 0.0, "");
        }
    }

    void process(const ProcessArgs &args) override;

    step_info_t steps[16][3];
    int base_note;
    float base_freq;
    unsigned int current_step = 0;
    unsigned int total_steps;
    unsigned int type;
    bool paused;
    bool clock_source_ext = false;
    float time_elapsed = 0.0;
    float step_duration;
    float ext_clock_last_value;
    float play_last_value;
    float reset_last_value[2];
    float tune;
};


void Micromacro::process(const ProcessArgs &args)
{
    step_duration = 15.0 / params[BPM].getValue();
    params[CLOCK_SOURCE].getValue() != 1.0 ? clock_source_ext = false : clock_source_ext = true;
    time_elapsed += args.sampleTime;
    total_steps = (unsigned int)(roundf(params[STEPS].getValue()));
    tune = params[TUNE].getValue();
    // Distance in semitones from middle A
    base_note = (roundf(params[BASE_OCTAVE].getValue()) * 12.0) + roundf(params[BASE_NOTE].getValue()) - 9.0;
    base_freq = tune * powf(TWO_1_12, base_note);
    type = (unsigned int)(params[INTERVALS].getValue());
    params[PLAY].getValue() != 1.0 ? paused = true : paused = false;
    play_last_value = params[PLAY].getValue();

    for (unsigned int s = 0; s < 16; s++)
    {
        s == current_step ? lights[STEP_LED_1 + s].value = 1.0 : lights[STEP_LED_1 + s].value = 0.0;
        params[GATE_1_1 + (s * 2)].getValue() == 0.0 ? steps[s][0].gate = false : steps[s][0].gate = true;
        params[GATE_1_2 + (s * 2)].getValue() == 0.0 ? steps[s][1].gate = false : steps[s][1].gate = true;
        params[GATE_1_3 + (s * 2)].getValue() == 0.0 ? steps[s][2].gate = false : steps[s][2].gate = true;

        switch (type)
        {
        case CHROMATIC:
            steps[s][0].pitch = base_freq * powf(TWO_1_12, roundf(params[PITCH_1_1 + (s * 2)].getValue()));
            steps[s][1].pitch = base_freq * powf(TWO_1_12, roundf(params[PITCH_1_2 + (s * 2)].getValue()));
            steps[s][2].pitch = base_freq * powf(TWO_1_12, roundf(params[PITCH_1_3 + (s * 2)].getValue()));
            break;
        case QUARTER_TONES:
            steps[s][0].pitch = base_freq * powf(TWO_1_24, roundf(params[PITCH_1_1 + (s * 2)].getValue()));
            steps[s][1].pitch = base_freq * powf(TWO_1_24, roundf(params[PITCH_1_2 + (s * 2)].getValue()));
            steps[s][2].pitch = base_freq * powf(TWO_1_24, roundf(params[PITCH_1_3 + (s * 2)].getValue()));
            break;
        case TWELFTH_TONES:
            steps[s][0].pitch = base_freq * powf(TWO_1_72, roundf(params[PITCH_1_1 + (s * 2)].getValue()));
            steps[s][1].pitch = base_freq * powf(TWO_1_72, roundf(params[PITCH_1_2 + (s * 2)].getValue()));
            steps[s][2].pitch = base_freq * powf(TWO_1_72, roundf(params[PITCH_1_3 + (s * 2)].getValue()));
            break;
        }
    }

    if (!paused)
    {
        for (unsigned int t = 0; t < 3; t++)
        {
            outputs[PITCH_OUT_1 + (unsigned int)(t * 2)].setVoltage(clamp(-1.0f * log2(440.0 / steps[current_step][t].pitch), -4.0f, 4.0f));
            outputs[GATE_OUT_1 + (unsigned int)(t * 2)].setVoltage(0.0);
        }

        lights[PLAY_LED].value = 1.0;

        if (!clock_source_ext)
        {
            if (time_elapsed >= step_duration)
            {
                time_elapsed = 0.0;
                if (++current_step >= total_steps)
                {
                    current_step = 0;
                }
                for (unsigned int t = 0; t < 3; t++)
                {
                    if (steps[current_step][t].gate)
                    {
                        outputs[GATE_OUT_1 + (unsigned int)(t * 2)].setVoltage(5.0);
                    }
                    else
                    {
                        outputs[GATE_OUT_1 + (unsigned int)(t * 2)].setVoltage(0.0);
                    }
                }
            }
        }
        else
        {
            if (inputs[EXT_CLOCK].getVoltage() != 0.0 && ext_clock_last_value == 0.0)
            {
                if (++current_step >= total_steps)
                {
                    current_step = 0;
                }
                for (unsigned int t = 0; t < 3; t++)
                {
                    if (steps[current_step][t].gate)
                    {
                        outputs[GATE_OUT_1 + (unsigned int)(t * 2)].setVoltage(5.0);
                    }
                    else
                    {
                        outputs[GATE_OUT_1 + (unsigned int)(t * 2)].setVoltage(0.0);
                    }
                }
            }
            ext_clock_last_value = inputs[EXT_CLOCK].getVoltage();
        }
    }
    else
    {
        lights[PLAY_LED].value = 0.0;
    }

    if ((inputs[CV_RESET].getVoltage() != 0.0 && reset_last_value[0] == 0.0) || (params[RESET].getValue() == 1.0 && reset_last_value[1] == 0.0))
    {
        current_step = 0;
    }
    reset_last_value[0] = inputs[CV_RESET].getVoltage();
    reset_last_value[1] = params[RESET].getValue();
}

struct MicromacroWidget : ModuleWidget
{
    MicromacroWidget(Micromacro *module)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/micromacro.svg")));
        
        addChild(createWidget<SonusScrew>(Vec(0, 0)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 0)));
        addChild(createWidget<SonusScrew>(Vec(0, 365)));
        addChild(createWidget<SonusScrew>(Vec(box.size.x - 15, 365)));

        addParam(createParam<SonusBigKnob>(Vec(45, 46.5), module, Micromacro::BPM));
        addParam(createParam<SonusBigSnapKnob>(Vec(133, 46.5), module, Micromacro::STEPS));
        addParam(createParam<SonusBigSnapKnob>(Vec(223, 46.5), module, Micromacro::BASE_OCTAVE));
        addParam(createParam<SonusBigSnapKnob>(Vec(311, 46.5), module, Micromacro::BASE_NOTE));
        addParam(createParam<SonusBigKnob>(Vec(399, 46.5), module, Micromacro::TUNE));

        addParam(createParam<NKK>(Vec(486, 49), module, Micromacro::INTERVALS));

        addInput(createInput<PJ301MPort>(Vec(592, 59), module, Micromacro::EXT_CLOCK));
        clock_select = createParam<CKSS>(Vec(635, 61.5), module, Micromacro::CLOCK_SOURCE);
        addParam(clock_select);
        //clock_select->randomizable = false;

        addInput(createInput<PJ301MPort>(Vec(688, 50.5), module, Micromacro::CV_RESET));
        addParam(createParam<CKD6>(Vec(728, 50.5), module, Micromacro::RESET));
        play_button = createParam<SonusLedButton>(Vec(728, 86.5), module, Micromacro::PLAY);
        addParam(play_button);
        //play_button->randomizable = false;

        for (unsigned int s = 0; s < 16; s++)
        {
            addChild(createLight<MediumLight<GreenLight>>(Vec(23.5 + (s * 44.0), 127), module, Micromacro::STEP_LED_1 + s));
        }

        for (unsigned int t = 0; t < 3; t++)
        {
            addOutput(createOutput<PJ301MPort>(Vec(730, 154 + (t * 70.0)), module, Micromacro::GATE_OUT_1 + (t * 2)));
            addOutput(createOutput<PJ301MPort>(Vec(770, 154 + (t * 70.0)), module, Micromacro::PITCH_OUT_1 + (t * 2)));

            for (unsigned int s = 0; s < 16; s++)
            {
                addParam(createParam<SonusSnapKnob>(Vec(10.0 + (s * 44.0), 149.0 + (t * 70.0)), module, Micromacro::PITCH_1_1 + (s * 2) + (t * 32)));
                addParam(createParam<SonusLedButton>(Vec(14.0 + (s * 44.0), 185.5 + (t * 70.0)), module, Micromacro::GATE_1_1 + (s * 2) + (t * 32)));
            }
        }

        addChild(createLight<MediumLight<GreenLight>>(Vec(757, 27), module, Micromacro::PLAY_LED));
    }

    ParamWidget *play_button;
    ParamWidget *clock_select;
};

Model *modelMicromacro = createModel<Micromacro, MicromacroWidget>("Micromacro");
