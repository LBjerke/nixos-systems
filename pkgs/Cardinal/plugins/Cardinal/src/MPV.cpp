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
#ifndef HEADLESS
# include "EmbedWidget.hpp"
# include "extra/ExternalWindow.hpp"
#endif

// --------------------------------------------------------------------------------------------------------------------

struct CardinalEmbedModule : Module {
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

    CardinalEmbedModule()
        : pcontext(static_cast<CardinalPluginContext*>(APP))
    {
        config(NUM_PARAMS, NUM_INPUTS, NUM_OUTPUTS, NUM_LIGHTS);

        configOutput(0, "Audio Left");
        configOutput(1, "Audio Right");
    }
};

// --------------------------------------------------------------------------------------------------------------------

#ifndef HEADLESS
struct CardinalEmbedWidget : ModuleWidget, ExternalWindow {
    CardinalEmbedModule* const module;
    CardinalPluginContext* const pcontext;
    EmbedWidget* embedWidget = nullptr;
    bool isEmbed = false;

    CardinalEmbedWidget(CardinalEmbedModule* const m)
        : ModuleWidget(),
          ExternalWindow(),
          module(m),
          pcontext(m != nullptr ? m->pcontext : nullptr)
    {
        setModule(module);
        setPanel(APP->window->loadSvg(asset::plugin(pluginInstance, "res/MPV.svg")));

        if (m != nullptr)
        {
            embedWidget = new EmbedWidget(box.size);
            addChild(embedWidget);
        }
    }

    ~CardinalEmbedWidget()
    {
        terminateAndWaitForExternalProcess();
    }

    void onContextCreate(const ContextCreateEvent& e) override
    {
        ModuleWidget::onContextCreate(e);
        widgetCreated();
    }

    void onContextDestroy(const ContextDestroyEvent& e) override
    {
        widgetDestroyed();
        ModuleWidget::onContextDestroy(e);
    }

    void onAdd(const AddEvent& e) override
    {
        ModuleWidget::onAdd(e);
        widgetCreated();
    }

    void onRemove(const RemoveEvent& e) override
    {
        widgetDestroyed();
        ModuleWidget::onRemove(e);
    }

    void widgetCreated()
    {
        DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);
        DISTRHO_SAFE_ASSERT_RETURN(pcontext->nativeWindowId != 0,);

        if (isEmbed)
            return;

        isEmbed = true;
        embedWidget->embedIntoRack(pcontext->nativeWindowId);
    }

    void widgetDestroyed()
    {
        DISTRHO_SAFE_ASSERT_RETURN(pcontext != nullptr,);

        if (! isEmbed)
            return;

        isEmbed = false;
        embedWidget->hide();
    }

    void appendContextMenu(ui::Menu* const menu) override
    {
        menu->addChild(new ui::MenuSeparator);

        struct LoadVideoFileItem : MenuItem {
            CardinalEmbedWidget* const self;

            LoadVideoFileItem(CardinalEmbedWidget* const s)
                : self(s)
            {
                text = "Load video file...";
            }

            void onAction(const event::Action&) override
            {
                WeakPtr<CardinalEmbedWidget> const self = this->self;
                async_dialog_filebrowser(false, nullptr, text.c_str(), [self](char* path)
                {
                    if (path == nullptr)
                        return;

                    if (self != nullptr)
                    {
                        char winIdStr[64];
                        std::snprintf(winIdStr, sizeof(winIdStr), "--wid=%lu",
                                      static_cast<ulong>(self->embedWidget->getNativeWindowId()));
                        const char* args[] = {
                            "mpv", "--no-audio", winIdStr, path, nullptr
                        };
                        self->terminateAndWaitForExternalProcess();
                        self->startExternalProcess(args);
                    }

                    std::free(path);
                });
            }
        };

        menu->addChild(new LoadVideoFileItem(this));
    }
};
#else
typedef ModuleWidget CardinalEmbedWidget;
#endif

// --------------------------------------------------------------------------------------------------------------------

Model* modelMPV = createModel<CardinalEmbedModule, CardinalEmbedWidget>("MPV");

// --------------------------------------------------------------------------------------------------------------------
