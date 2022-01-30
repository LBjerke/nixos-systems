/*
 * DISTRHO Cardinal Plugin
 * Copyright (C) 2021-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the LICENSE file.
 */

/**
 * This file contains a substantial amount of code from VCVRack's core/CV_MIDICC.cpp and core/MIDICC_CV.cpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */

#include "plugincontext.hpp"

#include <algorithm>

// -----------------------------------------------------------------------------------------------------------

USE_NAMESPACE_DISTRHO;

struct HostMIDICC : Module {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        ENUMS(CC_INPUTS, 16),
        CC_INPUT_CH_PRESSURE,
        CC_INPUT_PITCHBEND,
        NUM_INPUTS
    };
    enum OutputIds {
        ENUMS(CC_OUTPUT, 16),
        CC_OUTPUT_CH_PRESSURE,
        CC_OUTPUT_PITCHBEND,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;

    struct MidiInput {
        // Cardinal specific
        CardinalPluginContext* const pcontext;
        const MidiEvent* midiEvents;
        uint32_t midiEventsLeft;
        uint32_t midiEventFrame;
        int64_t lastBlockFrame;
        uint8_t channel;

        uint8_t chPressure[16];
        uint16_t pitchbend[16];

        // stuff from Rack
        /** [cc][channel] */
        uint8_t ccValues[128][16];
        /** When LSB is enabled for CC 0-31, the MSB is stored here until the LSB is received.
        [cc][channel]
        */
        uint8_t msbValues[32][16];
        int learningId;
        /** [cell][channel] */
        dsp::ExponentialFilter valueFilters[NUM_OUTPUTS][16];
        bool smooth;
        bool mpeMode;
        bool lsbMode;

        MidiInput(CardinalPluginContext* const pc)
            : pcontext(pc)
        {
            for (int i = 0; i < NUM_OUTPUTS; i++) {
                for (int c = 0; c < 16; c++) {
                    valueFilters[i][c].setTau(1 / 30.f);
                }
            }
            reset();
        }

        void reset()
        {
            midiEvents = nullptr;
            midiEventsLeft = 0;
            midiEventFrame = 0;
            lastBlockFrame = -1;
            channel = 0;

            for (int cc = 0; cc < 128; cc++) {
                for (int c = 0; c < 16; c++) {
                    ccValues[cc][c] = 0;
                }
            }
            for (int cc = 0; cc < 32; cc++) {
                for (int c = 0; c < 16; c++) {
                    msbValues[cc][c] = 0;
                }
            }
            for (int c = 0; c < 16; c++) {
                chPressure[c] = 0;
                pitchbend[c] = 8192;
            }
            learningId = -1;
            smooth = true;
            mpeMode = false;
            lsbMode = false;
        }

        bool process(const ProcessArgs& args, std::vector<rack::engine::Output>& outputs, int learnedCcs[16])
        {
            // Cardinal specific
            const int64_t blockFrame = pcontext->engine->getBlockFrame();
            const bool blockFrameChanged = lastBlockFrame != blockFrame;

            if (blockFrameChanged)
            {
                lastBlockFrame = blockFrame;

                midiEvents = pcontext->midiEvents;
                midiEventsLeft = pcontext->midiEventCount;
                midiEventFrame = 0;
            }

            while (midiEventsLeft != 0)
            {
                const MidiEvent& midiEvent(*midiEvents);

                if (midiEvent.frame > midiEventFrame)
                    break;

                ++midiEvents;
                --midiEventsLeft;

                const uint8_t* const data = midiEvent.size > MidiEvent::kDataSize
                                          ? midiEvent.dataExt
                                          : midiEvent.data;

                if (channel != 0 && data[0] < 0xF0)
                {
                    if ((data[0] & 0x0F) != (channel - 1))
                        continue;
                }

                const uint8_t status = data[0] & 0xF0;
                const uint8_t chan = data[0] & 0x0F;

                /**/ if (status == 0xD0)
                {
                    chPressure[chan] = data[1];
                }
                else if (status == 0xE0)
                {
                    pitchbend[chan] = (data[2] << 7) | data[1];
                }
                else if (status != 0xB0)
                {
                    continue;
                }

                // adapted from Rack
                const uint8_t c = mpeMode ? chan : 0;
                const uint8_t cc = data[1];
                const uint8_t value = data[2];

                // Learn
                if (learningId >= 0 && ccValues[cc][c] != value)
                {
                    learnedCcs[learningId] = cc;
                    learningId = -1;
                }

                if (lsbMode && cc < 32)
                {
                    // Don't set MSB yet. Wait for LSB to be received.
                    msbValues[cc][c] = value;
                }
                else if (lsbMode && 32 <= cc && cc < 64)
                {
                    // Apply MSB when LSB is received
                    ccValues[cc - 32][c] = msbValues[cc - 32][c];
                    ccValues[cc][c] = value;
                }
                else
                {
                    ccValues[cc][c] = value;
                }
            }

            ++midiEventFrame;

            // Rack stuff
            const int channels = mpeMode ? 16 : 1;

            for (int i = 0; i < 16; i++)
            {
                if (!outputs[CC_OUTPUT + i].isConnected())
                    continue;
                outputs[CC_OUTPUT + i].setChannels(channels);

                int cc = learnedCcs[i];

                for (int c = 0; c < channels; c++)
                {
                    int16_t cellValue = int16_t(ccValues[cc][c]) * 128;
                    if (lsbMode && cc < 32)
                        cellValue += ccValues[cc + 32][c];

                    // Maximum value for 14-bit CC should be MSB=127 LSB=0, not MSB=127 LSB=127, because this is the maximum value that 7-bit controllers can send.
                    const float value = static_cast<float>(cellValue) / (128.0f * 127.0f);

                    // Detect behavior from MIDI buttons.
                    if (smooth && std::fabs(valueFilters[i][c].out - value) < 1.f)
                    {
                        // Smooth value with filter
                        valueFilters[i][c].process(args.sampleTime, value);
                    }
                    else
                    {
                        // Jump value
                        valueFilters[i][c].out = value;
                    }

                    outputs[CC_OUTPUT + i].setVoltage(valueFilters[i][c].out * 10.f, c);
                }
            }

            if (outputs[CC_OUTPUT_CH_PRESSURE].isConnected())
            {
                outputs[CC_OUTPUT_CH_PRESSURE].setChannels(channels);

                for (int c = 0; c < channels; c++)
                {
                    const float value = static_cast<float>(chPressure[c]) / 128.0f;

                    // Detect behavior from MIDI buttons.
                    if (smooth && std::fabs(valueFilters[CC_OUTPUT_CH_PRESSURE][c].out - value) < 1.f)
                    {
                        // Smooth value with filter
                        valueFilters[CC_OUTPUT_CH_PRESSURE][c].process(args.sampleTime, value);
                    }
                    else
                    {
                        // Jump value
                        valueFilters[CC_OUTPUT_CH_PRESSURE][c].out = value;
                    }

                    outputs[CC_OUTPUT_CH_PRESSURE].setVoltage(valueFilters[CC_OUTPUT_CH_PRESSURE][c].out * 10.f, c);
                }
            }

            if (outputs[CC_OUTPUT_PITCHBEND].isConnected())
            {
                outputs[CC_OUTPUT_PITCHBEND].setChannels(channels);

                for (int c = 0; c < channels; c++)
                {
                    const float value = static_cast<float>(pitchbend[c]) / 16384.0f;

                    // Detect behavior from MIDI buttons.
                    if (smooth && std::fabs(valueFilters[CC_OUTPUT_PITCHBEND][c].out - value) < 1.f)
                    {
                        // Smooth value with filter
                        valueFilters[CC_OUTPUT_PITCHBEND][c].process(args.sampleTime, value);
                    }
                    else
                    {
                        // Jump value
                        valueFilters[CC_OUTPUT_PITCHBEND][c].out = value;
                    }

                    outputs[CC_OUTPUT_CH_PRESSURE].setVoltage(valueFilters[CC_OUTPUT_PITCHBEND][c].out * 10.f, c);
                }
            }

            return blockFrameChanged;
        }

    } midiInput;

    struct MidiOutput {
        // cardinal specific
        CardinalPluginContext* const pcontext;
        uint8_t channel = 0;

        // from Rack
        int lastValues[130];
        int64_t frame = 0;

        MidiOutput(CardinalPluginContext* const pc)
            : pcontext(pc)
        {
            reset();
        }

        void reset()
        {
            for (int n = 0; n < 130; ++n)
                lastValues[n] = -1;
        }

        void sendCC(const int cc, const int value)
        {
            if (lastValues[cc] == value)
                return;
            lastValues[cc] = value;
            midi::Message m;
            m.setStatus(0xb);
            m.setNote(cc);
            m.setValue(value);
            m.setFrame(frame);
            sendMessage(m);
        }

        void sendChanPressure(const int pressure)
        {
            if (lastValues[128] == pressure)
                return;
            lastValues[128] = pressure;
            midi::Message m;
            m.setStatus(0xd);
            m.setNote(pressure);
            m.setFrame(frame);
            sendMessage(m);
        }

        void sendPitchbend(const int pitchbend)
        {
            if (lastValues[129] == pitchbend)
                return;
            lastValues[129] = pitchbend;
            midi::Message m;
            m.setStatus(0xe);
            m.setNote(pitchbend & 0x7F);
            m.setValue(pitchbend >> 7);
            m.setFrame(frame);
            sendMessage(m);
        }

        void sendMessage(const midi::Message& message)
        {
            pcontext->writeMidiMessage(message, channel);
        }

    } midiOutput;

    int learnedCcs[16];

    HostMIDICC()
        : pcontext(static_cast<CardinalPluginContext*>(APP)),
          midiInput(pcontext),
          midiOutput(pcontext)
    {
        if (pcontext == nullptr)
            throw rack::Exception("Plugin context is null");

        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        for (int i = 0; i < 16; i++)
            configInput(CC_INPUTS + i, string::f("Cell %d", i + 1));

        configInput(CC_INPUT_CH_PRESSURE, "Channel pressure");
        configInput(CC_INPUT_PITCHBEND, "Pitchbend");

        for (int i = 0; i < 16; i++)
            configOutput(CC_OUTPUT + i, string::f("Cell %d", i + 1));

        configOutput(CC_OUTPUT_CH_PRESSURE, "Channel pressure");
        configOutput(CC_OUTPUT_PITCHBEND, "Pitchbend");

        onReset();
    }

    void onReset() override
    {
        for (int i = 0; i < 16; i++) {
            learnedCcs[i] = i + 1;
        }
        midiInput.reset();
        midiOutput.reset();
    }

    void process(const ProcessArgs& args) override
    {
        if (midiInput.process(args, outputs, learnedCcs))
            midiOutput.frame = 0;
        else
            ++midiOutput.frame;

        for (int i = 0; i < 16; i++)
        {
            int value = (int) std::round(inputs[CC_INPUTS + i].getVoltage() / 10.f * 127);
            value = clamp(value, 0, 127);
            midiOutput.sendCC(learnedCcs[i], value);
        }

        {
            int value = (int) std::round(inputs[CC_INPUT_CH_PRESSURE].getVoltage() / 10.f * 127);
            value = clamp(value, 0, 127);
            midiOutput.sendChanPressure(value);
        }

        {
            int value = (int) std::round(inputs[CC_INPUT_PITCHBEND].getVoltage() / 10.f * 16383);
            value = clamp(value, 0, 16383);
            midiOutput.sendPitchbend(value);
        }
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        // input and output
        if (json_t* const ccsJ = json_array())
        {
            for (int i = 0; i < 16; i++)
                json_array_append_new(ccsJ, json_integer(learnedCcs[i]));
            json_object_set_new(rootJ, "ccs", ccsJ);
        }

        // input only
        if (json_t* const valuesJ = json_array())
        {
            // Remember values so users don't have to touch MIDI controller knobs when restarting Rack
            for (int i = 0; i < 128; i++)
                // Note: Only save channel 0. Since MPE mode won't be commonly used, it's pointless to save all 16 channels.
                json_array_append_new(valuesJ, json_integer(midiInput.ccValues[i][0]));
            json_object_set_new(rootJ, "values", valuesJ);
        }

        json_object_set_new(rootJ, "smooth", json_boolean(midiInput.smooth));
        json_object_set_new(rootJ, "mpeMode", json_boolean(midiInput.mpeMode));
        json_object_set_new(rootJ, "lsbMode", json_boolean(midiInput.lsbMode));

        // separate
        json_object_set_new(rootJ, "inputChannel", json_integer(midiInput.channel));
        json_object_set_new(rootJ, "outputChannel", json_integer(midiOutput.channel));

        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        // input and output
        if (json_t* const ccsJ = json_object_get(rootJ, "ccs"))
        {
            for (int i = 0; i < 16; i++)
            {
                if (json_t* const ccJ = json_array_get(ccsJ, i))
                    learnedCcs[i] = json_integer_value(ccJ);
                else
                    learnedCcs[i] = i + 1;
            }
        }

        // input only
        if (json_t* const valuesJ = json_object_get(rootJ, "values"))
        {
            for (int i = 0; i < 128; i++) {
                if (json_t* const valueJ = json_array_get(valuesJ, i))
                    midiInput.ccValues[i][0] = json_integer_value(valueJ);
            }
        }

        if (json_t* const smoothJ = json_object_get(rootJ, "smooth"))
            midiInput.smooth = json_boolean_value(smoothJ);

        if (json_t* const mpeModeJ = json_object_get(rootJ, "mpeMode"))
            midiInput.mpeMode = json_boolean_value(mpeModeJ);

        if (json_t* const lsbEnabledJ = json_object_get(rootJ, "lsbMode"))
            midiInput.lsbMode = json_boolean_value(lsbEnabledJ);

        // separate
        if (json_t* const inputChannelJ = json_object_get(rootJ, "inputChannel"))
            midiInput.channel = json_integer_value(inputChannelJ);

        if (json_t* const outputChannelJ = json_object_get(rootJ, "outputChannel"))
            midiOutput.channel = json_integer_value(outputChannelJ) & 0x0F;
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
/**
 * Based on VCVRack's CcChoice as defined in src/core/plugin.hpp
 * Copyright (C) 2016-2021 VCV.
 *
 * This program is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of
 * the License, or (at your option) any later version.
 */
struct CardinalCcChoice : CardinalLedDisplayChoice {
    HostMIDICC* const module;
    const int id;
    int focusCc = -1;

    CardinalCcChoice(HostMIDICC* const m, const int i)
      : CardinalLedDisplayChoice(),
        module(m),
        id(i)
    {
        // Module browser setup
        if (m == nullptr)
        {
            text = string::f("%d", i+1);
        }
    }

    void step() override
    {
        int cc;

        if (module == nullptr)
        {
            cc = id;
        }
        else if (module->midiInput.learningId == id)
        {
            cc = focusCc;
            color.a = 0.5f;
        }
        else
        {
            cc = module->learnedCcs[id];
            color.a = 1.0f;

            // Cancel focus if no longer learning
            if (APP->event->getSelectedWidget() == this)
                APP->event->setSelectedWidget(NULL);
        }

        // Set text
        if (cc < 0)
            text = "--";
        else
            text = string::f("%d", cc);
    }

    void onSelect(const SelectEvent& e) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

        module->midiInput.learningId = id;
        focusCc = -1;
        e.consume(this);
    }

    void onDeselect(const DeselectEvent&) override
    {
        DISTRHO_SAFE_ASSERT_RETURN(module != nullptr,);

        if (module->midiInput.learningId == id)
        {
            if (0 <= focusCc && focusCc < 128)
                module->learnedCcs[id] = focusCc;
            module->midiInput.learningId = -1;
        }
    }

    void onSelectText(const SelectTextEvent& e) override
    {
        int c = e.codepoint;

        if ('0' <= c && c <= '9')
        {
            if (focusCc < 0)
                focusCc = 0;
            focusCc = focusCc * 10 + (c - '0');
        }

        if (focusCc >= 128)
            focusCc = -1;

        e.consume(this);
    }

    void onSelectKey(const SelectKeyEvent& e) override
    {
        if (e.key != GLFW_KEY_ENTER && e.key != GLFW_KEY_KP_ENTER)
            return;
        if (e.action != GLFW_PRESS)
            return;
        if (e.mods & RACK_MOD_MASK)
            return;

        DeselectEvent eDeselect;
        onDeselect(eDeselect);
        APP->event->selectedWidget = NULL;
        e.consume(this);
    }
};

struct CCGridDisplay : Widget {
    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, 0, 0, box.size.x, box.size.y, 4);
        nvgFillColor(args.vg, nvgRGB(0, 0, 0));
        nvgFill(args.vg);

        Widget::draw(args);
    }

    void setModule(HostMIDICC* const module)
    {
        LedDisplaySeparator* hSeparators[6];
        LedDisplaySeparator* vSeparators[3];
        LedDisplayChoice* choices[3][6];

        // Add vSeparators
        for (int x = 0; x < 3; ++x)
        {
            vSeparators[x] = new LedDisplaySeparator;
            vSeparators[x]->box.pos = Vec(box.size.x / 3 * (x+1), 0.0f);
            vSeparators[x]->box.size = Vec(1.0f, box.size.y);
            addChild(vSeparators[x]);
        }

        // Add hSeparators and choice widgets
        for (int y = 0; y < 6; ++y)
        {
            hSeparators[y] = new LedDisplaySeparator;
            hSeparators[y]->box.pos = Vec(0.0f, box.size.y / 6 * (y+1));
            hSeparators[y]->box.size = Vec(box.size.x, 1.0f);
            addChild(hSeparators[y]);

            for (int x = 0; x < 3; ++x)
            {
                const int id = 6 * x + y;

                switch (id)
                {
                case 16:
                    choices[x][y] = new CardinalLedDisplayChoice("Ch.press");
                    break;
                case 17:
                    choices[x][y] = new CardinalLedDisplayChoice("Pbend");
                    break;
                default:
                    choices[x][y] = new CardinalCcChoice(module, id);
                    break;
                }

                choices[x][y]->box.pos = Vec(box.size.x / 3 * x, box.size.y / 6 * y);
                choices[x][y]->box.size = Vec(box.size.x / 3, box.size.y / 6);
                addChild(choices[x][y]);
            }
        }
	}
};

struct HostMIDICCWidget : ModuleWidget {
    static constexpr const float startX_In = 14.0f;
    static constexpr const float startX_Out = 115.0f;
    static constexpr const float startY = 190.0f;
    static constexpr const float padding = 29.0f;

    HostMIDICC* const module;

    HostMIDICCWidget(HostMIDICC* const m)
        : module(m)
    {
        setModule(m);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/HostMIDICC.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        for (int i=0; i<18; ++i)
        {
            const float x = startX_In + int(i / 6) * padding;
            const float y = startY + int(i % 6) * padding;
            addInput(createInput<PJ301MPort>(Vec(x, y), module, i));
        }

        for (int i=0; i<18; ++i)
        {
            const float x = startX_Out + int(i / 6) * padding;
            const float y = startY + int(i % 6) * padding;
            addOutput(createOutput<PJ301MPort>(Vec(x, y), module, i));
        }

        CCGridDisplay* const display = createWidget<CCGridDisplay>(Vec(startX_In - 3.0f, 70.0f));
        display->box.size = Vec(box.size.x - startX_In * 2.0f + 6.0f, startY - 74.0f - 9.0f);
        display->setModule(m);
        addChild(display);
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19), nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg, startX_Out - 2.5f, startY - 2.0f, padding * 3, padding * 6, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        ModuleWidget::draw(args);
    }

    void appendContextMenu(Menu* const menu) override
    {
        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Input"));

        menu->addChild(createBoolPtrMenuItem("Smooth CC", "", &module->midiInput.smooth));
        menu->addChild(createBoolPtrMenuItem("MPE mode", "", &module->midiInput.mpeMode));
        menu->addChild(createBoolPtrMenuItem("14-bit CC 0-31 / 32-63", "", &module->midiInput.lsbMode));

        struct InputChannelItem : MenuItem {
            HostMIDICC* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (int c = 0; c <= 16; c++) {
                    menu->addChild(createCheckMenuItem((c == 0) ? "All" : string::f("%d", c), "",
                        [=]() {return module->midiInput.channel == c;},
                        [=]() {module->midiInput.channel = c;}
                    ));
                }
                return menu;
            }
        };
        InputChannelItem* const inputChannelItem = new InputChannelItem;
        inputChannelItem->text = "MIDI channel";
        inputChannelItem->rightText = (module->midiInput.channel ? string::f("%d", module->midiInput.channel) : "All")
                                    + "  " + RIGHT_ARROW;
        inputChannelItem->module = module;
        menu->addChild(inputChannelItem);

        menu->addChild(new MenuSeparator);
        menu->addChild(createMenuLabel("MIDI Output"));

        struct OutputChannelItem : MenuItem {
            HostMIDICC* module;
            Menu* createChildMenu() override {
                Menu* menu = new Menu;
                for (uint8_t c = 0; c < 16; c++) {
                    menu->addChild(createCheckMenuItem(string::f("%d", c+1), "",
                        [=]() {return module->midiOutput.channel == c;},
                        [=]() {module->midiOutput.channel = c;}
                    ));
                }
                return menu;
            }
        };
        OutputChannelItem* const outputChannelItem = new OutputChannelItem;
        outputChannelItem->text = "MIDI channel";
        outputChannelItem->rightText = string::f("%d", module->midiOutput.channel+1) + "  " + RIGHT_ARROW;
        outputChannelItem->module = module;
        menu->addChild(outputChannelItem);
    }
};
#else
typedef ModuleWidget HostMIDICCWidget;
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelHostMIDICC = createModel<HostMIDICC, HostMIDICCWidget>("HostMIDICC");

// --------------------------------------------------------------------------------------------------------------------
