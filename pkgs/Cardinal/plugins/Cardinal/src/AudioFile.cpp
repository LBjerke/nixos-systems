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

#include "plugincontext.hpp"
#include "extra/Thread.hpp"

#include "dgl/src/nanovg/nanovg.h"

#include "CarlaNativePlugin.h"

#ifndef HEADLESS
# include "ImGuiWidget.hpp"
# include "ghc/filesystem.hpp"
#endif

#define BUFFER_SIZE 128

// generates a warning if this is defined as anything else
#define CARLA_API

// --------------------------------------------------------------------------------------------------------------------

std::size_t carla_getNativePluginCount() noexcept;
const NativePluginDescriptor* carla_getNativePluginDescriptor(const std::size_t index) noexcept;

// --------------------------------------------------------------------------------------------------------------------

using namespace CarlaBackend;

static uint32_t host_get_buffer_size(NativeHostHandle);
static double host_get_sample_rate(NativeHostHandle);
static bool host_is_offline(NativeHostHandle);
static const NativeTimeInfo* host_get_time_info(NativeHostHandle handle);
static bool host_write_midi_event(NativeHostHandle handle, const NativeMidiEvent* event);
static void host_ui_midi_program_changed(NativeHostHandle handle, uint8_t channel, uint32_t bank, uint32_t program);
static void host_ui_custom_data_changed(NativeHostHandle handle, const char* key, const char* value);
static intptr_t host_dispatcher(NativeHostHandle handle, NativeHostDispatcherOpcode opcode, int32_t index, intptr_t value, void* ptr, float opt);

static void host_ui_parameter_changed(NativeHostHandle, uint32_t, float) {}
static void host_ui_midi_program_changed(NativeHostHandle, uint8_t, uint32_t, uint32_t) {}
static void host_ui_custom_data_changed(NativeHostHandle, const char*, const char*) {}
static const char* host_ui_open_file(NativeHostHandle, bool, const char*, const char*) { return nullptr; }
static const char* host_ui_save_file(NativeHostHandle, bool, const char*, const char*) { return nullptr; }
static void host_ui_closed(NativeHostHandle) {}

// --------------------------------------------------------------------------------------------------------------------

struct CarlaInternalPluginModule : Module, Thread {
    enum ParamIds {
        NUM_PARAMS
    };
    enum InputIds {
        NUM_INPUTS
    };
    enum OutputIds {
        AUDIO_OUTPUT1,
        AUDIO_OUTPUT2,
        NUM_OUTPUTS
    };
    enum LightIds {
        NUM_LIGHTS
    };

    CardinalPluginContext* const pcontext;

    const NativePluginDescriptor* fCarlaPluginDescriptor = nullptr;
    NativePluginHandle fCarlaPluginHandle = nullptr;
    NativeHostDescriptor fCarlaHostDescriptor = {};
    NativeTimeInfo fCarlaTimeInfo;

    float dataOut[NUM_OUTPUTS][BUFFER_SIZE];
    float* dataOutPtr[NUM_OUTPUTS];
    unsigned audioDataFill = 0;
    int64_t lastBlockFrame = -1;
    bool fileChanged = false;
    std::string currentFile;

    struct {
        float preview[108];
        uint channels; // 4
        uint bitDepth; // 6
        uint sampleRate; // 7
        uint length; // 8
        float position; // 9
    } audioInfo;

    CarlaInternalPluginModule()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configOutput(0, "Audio Left");
        configOutput(1, "Audio Right");

        dataOutPtr[0] = dataOut[0];
        dataOutPtr[1] = dataOut[1];

        std::memset(dataOut, 0, sizeof(dataOut));
        std::memset(&audioInfo, 0, sizeof(audioInfo));

        for (std::size_t i=0, count=carla_getNativePluginCount(); i<count; ++i)
        {
            const NativePluginDescriptor* const desc = carla_getNativePluginDescriptor(i);

            if (std::strcmp(desc->label, "audiofile") != 0)
                continue;

            fCarlaPluginDescriptor = desc;
            break;
        }

        DISTRHO_SAFE_ASSERT_RETURN(fCarlaPluginDescriptor != nullptr,);

        memset(&fCarlaHostDescriptor, 0, sizeof(fCarlaHostDescriptor));
        memset(&fCarlaTimeInfo, 0, sizeof(fCarlaTimeInfo));

        fCarlaHostDescriptor.handle = this;
        fCarlaHostDescriptor.resourceDir = "";
        fCarlaHostDescriptor.uiName = "Cardinal";

        fCarlaHostDescriptor.get_buffer_size = host_get_buffer_size;
        fCarlaHostDescriptor.get_sample_rate = host_get_sample_rate;
        fCarlaHostDescriptor.is_offline = host_is_offline;

        fCarlaHostDescriptor.get_time_info = host_get_time_info;
        fCarlaHostDescriptor.write_midi_event = host_write_midi_event;
        fCarlaHostDescriptor.ui_parameter_changed = host_ui_parameter_changed;
        fCarlaHostDescriptor.ui_midi_program_changed = host_ui_midi_program_changed;
        fCarlaHostDescriptor.ui_custom_data_changed = host_ui_custom_data_changed;
        fCarlaHostDescriptor.ui_closed = host_ui_closed;
        fCarlaHostDescriptor.ui_open_file = host_ui_open_file;
        fCarlaHostDescriptor.ui_save_file = host_ui_save_file;
        fCarlaHostDescriptor.dispatcher = host_dispatcher;

        fCarlaPluginHandle = fCarlaPluginDescriptor->instantiate(&fCarlaHostDescriptor);
        DISTRHO_SAFE_ASSERT_RETURN(fCarlaPluginHandle != nullptr,);

        fCarlaPluginDescriptor->activate(fCarlaPluginHandle);

        // host-sync disabled by default
        fCarlaPluginDescriptor->set_parameter_value(fCarlaPluginHandle, 1, 0.0f);

        startThread();
    }

    ~CarlaInternalPluginModule() override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        stopThread(-1);
        fCarlaPluginDescriptor->deactivate(fCarlaPluginHandle);
        fCarlaPluginDescriptor->cleanup(fCarlaPluginHandle);
    }

    void run() override
    {
        while (!shouldThreadExit())
        {
            d_msleep(500);
            fCarlaPluginDescriptor->dispatcher(fCarlaPluginHandle, NATIVE_PLUGIN_OPCODE_IDLE, 0, 0, nullptr, 0.0f);
        }
    }

    const NativeTimeInfo* hostGetTimeInfo() const noexcept
    {
        return &fCarlaTimeInfo;
    }

    intptr_t hostDispatcher(const NativeHostDispatcherOpcode opcode,
                            const int32_t index, const intptr_t value, void* const ptr, const float opt)
    {
        switch (opcode)
        {
        // cannnot be supported
        case NATIVE_HOST_OPCODE_HOST_IDLE:
            break;
        // other stuff
        case NATIVE_HOST_OPCODE_NULL:
        case NATIVE_HOST_OPCODE_UPDATE_PARAMETER:
        case NATIVE_HOST_OPCODE_UPDATE_MIDI_PROGRAM:
        case NATIVE_HOST_OPCODE_RELOAD_PARAMETERS:
        case NATIVE_HOST_OPCODE_RELOAD_MIDI_PROGRAMS:
        case NATIVE_HOST_OPCODE_RELOAD_ALL:
        case NATIVE_HOST_OPCODE_UI_UNAVAILABLE:
        case NATIVE_HOST_OPCODE_INTERNAL_PLUGIN:
        case NATIVE_HOST_OPCODE_QUEUE_INLINE_DISPLAY:
        case NATIVE_HOST_OPCODE_UI_TOUCH_PARAMETER:
        case NATIVE_HOST_OPCODE_UI_RESIZE:
            break;
        case NATIVE_HOST_OPCODE_PREVIEW_BUFFER_DATA:
            std::memcpy(audioInfo.preview, ptr, sizeof(audioInfo.preview));
            break;
        case NATIVE_HOST_OPCODE_GET_FILE_PATH:
        case NATIVE_HOST_OPCODE_REQUEST_IDLE:
            break;
        }

        return 0;
    }

    json_t* dataToJson() override
    {
        json_t* const rootJ = json_object();
        DISTRHO_SAFE_ASSERT_RETURN(rootJ != nullptr, nullptr);

        json_object_set_new(rootJ, "filepath", json_string(currentFile.c_str()));
        return rootJ;
    }

    void dataFromJson(json_t* const rootJ) override
    {
        if (json_t* const filepathJ = json_object_get(rootJ, "filepath"))
        {
            const char* const filepath = json_string_value(filepathJ);

            if (filepath[0] != '\0')
            {
                currentFile = filepath;
                fileChanged = true;

                if (fCarlaPluginHandle != nullptr)
                    fCarlaPluginDescriptor->set_custom_data(fCarlaPluginHandle, "file", filepath);

                return;
            }
        }

        currentFile.clear();
        fileChanged = true;
    }

    void process(const ProcessArgs&) override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        const unsigned k = audioDataFill++;

        outputs[0].setVoltage(dataOut[0][k] * 10.0f);
        outputs[1].setVoltage(dataOut[1][k] * 10.0f);

        if (audioDataFill == BUFFER_SIZE)
        {
            const int64_t blockFrame = pcontext->engine->getBlockFrame();

            // Update time position if running a new audio block
            if (lastBlockFrame != blockFrame)
            {
                lastBlockFrame = blockFrame;
                fCarlaTimeInfo.playing = pcontext->playing;
                fCarlaTimeInfo.frame = pcontext->frame;
            }
            // or advance time by BUFFER_SIZE frames if still under the same audio block
            else if (fCarlaTimeInfo.playing)
            {
                fCarlaTimeInfo.frame += BUFFER_SIZE;
            }

            audioDataFill = 0;
            fCarlaPluginDescriptor->process(fCarlaPluginHandle, nullptr, dataOutPtr, BUFFER_SIZE, nullptr, 0);

            audioInfo.channels = fCarlaPluginDescriptor->get_parameter_value(fCarlaPluginHandle, 4);
            audioInfo.bitDepth = fCarlaPluginDescriptor->get_parameter_value(fCarlaPluginHandle, 6);
            audioInfo.sampleRate = fCarlaPluginDescriptor->get_parameter_value(fCarlaPluginHandle, 7);
            audioInfo.length = fCarlaPluginDescriptor->get_parameter_value(fCarlaPluginHandle,  8);
            audioInfo.position = fCarlaPluginDescriptor->get_parameter_value(fCarlaPluginHandle, 9);
        }
    }

    void onSampleRateChange(const SampleRateChangeEvent& e) override
    {
        if (fCarlaPluginHandle == nullptr)
            return;

        fCarlaPluginDescriptor->deactivate(fCarlaPluginHandle);
        fCarlaPluginDescriptor->dispatcher(fCarlaPluginHandle, NATIVE_PLUGIN_OPCODE_SAMPLE_RATE_CHANGED,
                                           0, 0, nullptr, e.sampleRate);
        fCarlaPluginDescriptor->activate(fCarlaPluginHandle);
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(CarlaInternalPluginModule)
};

// -----------------------------------------------------------------------------------------------------------

static uint32_t host_get_buffer_size(NativeHostHandle)
{
    return BUFFER_SIZE;
}

static double host_get_sample_rate(const NativeHostHandle handle)
{
    CardinalPluginContext* const pcontext = static_cast<CarlaInternalPluginModule*>(handle)->pcontext;
    DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr, 48000.0);
    return pcontext->sampleRate;
}

static bool host_is_offline(NativeHostHandle)
{
    return false;
}

static const NativeTimeInfo* host_get_time_info(const NativeHostHandle handle)
{
    return static_cast<CarlaInternalPluginModule*>(handle)->hostGetTimeInfo();
}

static bool host_write_midi_event(NativeHostHandle, const NativeMidiEvent*)
{
    return false;
}

static intptr_t host_dispatcher(const NativeHostHandle handle, const NativeHostDispatcherOpcode opcode,
                                const int32_t index, const intptr_t value, void* const ptr, const float opt)
{
    return static_cast<CarlaInternalPluginModule*>(handle)->hostDispatcher(opcode, index, value, ptr, opt);
}

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct AudioFileListWidget : ImGuiWidget {
    CarlaInternalPluginModule* const module;

    bool showError = false;
    String errorMessage;

    struct ghcFile { std::string full, base; };
    std::string currentDirectory;
    std::vector<ghcFile> currentFiles;
    size_t selectedFile = (size_t)-1;

    AudioFileListWidget(CarlaInternalPluginModule* const m)
        : ImGuiWidget(),
          module(m)
    {
        if (module->fileChanged)
            reloadDir();
    }

    void drawImGui() override
    {
        const float scaleFactor = getScaleFactor();

        const int flags = ImGuiWindowFlags_NoSavedSettings
                        | ImGuiWindowFlags_NoTitleBar
                        | ImGuiWindowFlags_NoResize
                        | ImGuiWindowFlags_NoCollapse
                        | ImGuiWindowFlags_NoScrollbar
                        | ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(box.size.x * scaleFactor, box.size.y * scaleFactor));

        if (ImGui::Begin("Plugin List", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize))
        {
            if (showError)
            {
                showError = false;
                ImGui::OpenPopup("Audio File Error");
            }

            if (ImGui::BeginPopupModal("Audio File Error", nullptr, flags))
            {
                ImGui::TextWrapped("Failed to load audio file, error was:\n%s", errorMessage.buffer());

                ImGui::Separator();

                if (ImGui::Button("Ok"))
                    ImGui::CloseCurrentPopup();

                ImGui::EndPopup();
            }
            else if (ImGui::BeginTable("pluginlist", 1, ImGuiTableFlags_NoSavedSettings))
            {
                for (size_t i=0, count=currentFiles.size(); i < count; ++i)
                {
                    bool wasSelected = selectedFile == i;
                    bool selected = wasSelected;
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    ImGui::Selectable(currentFiles[i].base.c_str(), &selected);

                    if (selected && ! wasSelected)
                    {
                        selectedFile = i;
                        module->currentFile = currentFiles[i].full;
                        module->fCarlaPluginDescriptor->set_custom_data(module->fCarlaPluginHandle, "file", currentFiles[i].full.c_str());
                    }
                }

                ImGui::EndTable();
            }
        }

        ImGui::End();
    }

    void step() override
    {
        if (module->fileChanged)
            reloadDir();

        ImGuiWidget::step();
    }

    void reloadDir()
    {
        module->fileChanged = false;

        currentFiles.clear();
        selectedFile = (size_t)-1;

        static constexpr const char* const supportedExtensions[] = {
       #ifdef HAVE_SNDFILE
            ".aif",".aifc",".aiff",".au",".bwf",".flac",".htk",".iff",".mat4",".mat5",".oga",".ogg;"
            ".paf",".pvf",".pvf5",".sd2",".sf",".snd",".svx",".vcc",".w64",".wav",".xi",
       #endif
            ".mp3"
        };

        using namespace ghc::filesystem;
        currentDirectory = path(module->currentFile).parent_path().string();

        directory_iterator it(currentDirectory);
        size_t index = 0;
        for (directory_iterator itb = begin(it), ite=end(it); itb != ite; ++itb)
        {
            if (! itb->is_regular_file())
                continue;
            const path filepath = itb->path();
            const path extension = filepath.extension();
            for (size_t i=0; i<ARRAY_SIZE(supportedExtensions); ++i)
            {
                if (extension.compare(supportedExtensions[i]) == 0)
                {
                    if (filepath.compare(module->currentFile) == 0)
                        selectedFile = index;
                    currentFiles.push_back({ filepath.string(), filepath.filename().string() });
                    ++index;
                    break;
                }
            }
        }
    }
};

struct AudioFileWidget : ModuleWidget {
    static constexpr const float padding = 29.0f;

    CarlaInternalPluginModule* const module;
    bool idleCallbackActive = false;
    bool visible = false;
    float lastPosition = 0.0f;

    AudioFileWidget(CarlaInternalPluginModule* const m)
        : ModuleWidget(),
          module(m)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/AudioFile.svg")));

        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
        addChild(createWidget<ScrewBlack>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
        addChild(createWidget<ScrewBlack>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

        addOutput(createOutput<PJ301MPort>(Vec(box.size.x - RACK_GRID_WIDTH * 5/2 - padding,
                                               RACK_GRID_HEIGHT - RACK_GRID_WIDTH - padding), module, 0));

        addOutput(createOutput<PJ301MPort>(Vec(box.size.x - RACK_GRID_WIDTH * 5/2,
                                               RACK_GRID_HEIGHT - RACK_GRID_WIDTH - padding), module, 1));

        if (m != nullptr)
        {
            AudioFileListWidget* const listw = new AudioFileListWidget(m);
            listw->box.pos.x = 0;
            listw->box.pos.y = 71.0f;
            listw->box.size.x = box.size.x;
            listw->box.size.y = box.size.y / 2 - 20;
            addChild(listw);
        }
    }

    void drawLayer(const DrawArgs& args, int layer) override
    {
        if (layer != 1)
            return ModuleWidget::drawLayer(args, layer);

        const Rect audioPreviewPos = Rect(8, box.size.y - 51 - 80, box.size.x - 16, 80);
        const float alpha = 1.0f - (0.5f - settings::rackBrightness * 0.5f);
        char textInfo[0xff];

        if (module != nullptr && module->audioInfo.channels != 0)
        {
            const float audioPreviewBarHeight = audioPreviewPos.size.y - 20;
            const size_t position = (module->audioInfo.position * 0.01f) * ARRAY_SIZE(module->audioInfo.preview);

            nvgFillColor(args.vg, nvgRGBAf(0.839f, 0.459f, 0.086f, alpha));

            for (size_t i=0; i<ARRAY_SIZE(module->audioInfo.preview); ++i)
            {
                const float value = module->audioInfo.preview[i];
                const float height = std::max(0.01f, value * audioPreviewBarHeight);
                const float y = audioPreviewPos.pos.y + audioPreviewBarHeight - height;

                if (position == i)
                    nvgFillColor(args.vg, nvgRGBAf(1.0f, 1.0f, 1.0f, alpha));

                nvgBeginPath(args.vg);
                nvgRect(args.vg,
                        audioPreviewPos.pos.x + 3 + 3 * i, y + 2, 2, height);
                nvgFill(args.vg);
            }

            std::snprintf(textInfo, sizeof(textInfo), "%s %d-Bit, %.1fkHz, %dm%02ds",
                          module->audioInfo.channels == 1 ? "Mono" : module->audioInfo.channels == 2 ? "Stereo" : "Other",
                          module->audioInfo.bitDepth,
                          static_cast<float>(module->audioInfo.sampleRate)/1000.0f,
                          module->audioInfo.length / 60,
                          module->audioInfo.length % 60);
        }
        else
        {
            std::strcpy(textInfo, "No file loaded");
        }

        nvgFillColor(args.vg, nvgRGBAf(1.0f, 1.0f, 1.0f, alpha));
        nvgFontFaceId(args.vg, 0);
        nvgFontSize(args.vg, 13);
        nvgTextAlign(args.vg, NVG_ALIGN_LEFT);

        nvgText(args.vg,
                audioPreviewPos.pos.x + 4,
                audioPreviewPos.pos.y + audioPreviewPos.size.y - 6,
                textInfo, nullptr);
    }

    void draw(const DrawArgs& args) override
    {
        nvgBeginPath(args.vg);
        nvgRect(args.vg, 0, 0, box.size.x, box.size.y);
        nvgFillPaint(args.vg, nvgLinearGradient(args.vg, 0, 0, 0, box.size.y,
                                                nvgRGB(0x18, 0x19, 0x19),
                                                nvgRGB(0x21, 0x22, 0x22)));
        nvgFill(args.vg);

        const Rect audioPreviewPos = Rect(8, box.size.y - 51 - 80, box.size.x - 16, 80);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg,
                       audioPreviewPos.pos.x, audioPreviewPos.pos.y,
                       audioPreviewPos.size.x, audioPreviewPos.size.y, 4.0f);
        nvgFillColor(args.vg, nvgRGB(0x75, 0x17, 0x00));
        nvgFill(args.vg);
        nvgStrokeWidth(args.vg, 2.0f);
        nvgStrokeColor(args.vg, nvgRGB(0xd6, 0x75, 0x16));
        nvgStroke(args.vg);

        nvgBeginPath(args.vg);
        nvgRoundedRect(args.vg,
                       box.size.x - RACK_GRID_WIDTH * 5/2 - padding - 3.0f,
                       RACK_GRID_HEIGHT - RACK_GRID_WIDTH - padding - 2.0f,
                       padding * CarlaInternalPluginModule::NUM_OUTPUTS,
                       padding, 4);
        nvgFillColor(args.vg, nvgRGB(0xd0, 0xd0, 0xd0));
        nvgFill(args.vg);

        ModuleWidget::draw(args);
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        menu->addChild(new ui::MenuSeparator);

        struct LoadAudioFileItem : MenuItem {
            CarlaInternalPluginModule* const module;

            LoadAudioFileItem(CarlaInternalPluginModule* const m)
                : module(m)
            {
                text = "Load audio file...";
            }

            void onAction(const event::Action&) override
            {
                CarlaInternalPluginModule* const module = this->module;
                async_dialog_filebrowser(false, nullptr, text.c_str(), [module](char* path)
                {
                    if (path == nullptr)
                        return;

                    module->currentFile = path;
                    module->fileChanged = true;
                    module->fCarlaPluginDescriptor->set_custom_data(module->fCarlaPluginHandle, "file", path);
                    std::free(path);
                });
            }
        };

        menu->addChild(new LoadAudioFileItem(module));
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AudioFileWidget)
};
#else
typedef ModuleWidget AudioFileWidget;
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelAudioFile = createModel<CarlaInternalPluginModule, AudioFileWidget>("AudioFile");

// --------------------------------------------------------------------------------------------------------------------
