/*
 * Simple Gain audio effect for DISTRHO Plugin Framework (DPF)
 * SPDX-License-Identifier: MIT
 *
 * Copyright (C) 2021 Jean Pierre Cimalando <jp-dev@inbox.ru>
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/**
   The plugin name.@n
   This is used to identify your plugin before a Plugin instance can be created.
   @note This macro is required.
 */
#define DISTRHO_PLUGIN_NAME "ImGuiSimpleGain"

/**
   Number of audio inputs the plugin has.
   @note This macro is required.
 */
#define DISTRHO_PLUGIN_NUM_INPUTS 2

/**
   Number of audio outputs the plugin has.
   @note This macro is required.
 */
#define DISTRHO_PLUGIN_NUM_OUTPUTS 2

/**
   The plugin URI when exporting in LV2 format.
   @note This macro is required.
 */
#define DISTRHO_PLUGIN_URI "http://distrho.sf.net/examples/imguisimplegain"

/**
   Wherever the plugin has a custom %UI.
   @see DISTRHO_UI_USE_NANOVG
   @see UI
 */
#define DISTRHO_PLUGIN_HAS_UI 1

/**
   Wherever the plugin processing is realtime-safe.@n
   TODO - list rtsafe requirements
 */
#define DISTRHO_PLUGIN_IS_RT_SAFE 1

/**
   Wherever the plugin is a synth.@n
   @ref DISTRHO_PLUGIN_WANT_MIDI_INPUT is automatically enabled when this is too.
   @see DISTRHO_PLUGIN_WANT_MIDI_INPUT
 */
#define DISTRHO_PLUGIN_IS_SYNTH 0

/**
   Enable direct access between the %UI and plugin code.
   @see UI::getPluginInstancePointer()
   @note DO NOT USE THIS UNLESS STRICTLY NECESSARY!!
         Try to avoid it at all costs!
 */
#define DISTRHO_PLUGIN_WANT_DIRECT_ACCESS 0

/**
   Wherever the plugin introduces latency during audio or midi processing.
   @see Plugin::setLatency(uint32_t)
 */
#define DISTRHO_PLUGIN_WANT_LATENCY 0

/**
   Wherever the plugin wants MIDI input.@n
   This is automatically enabled if @ref DISTRHO_PLUGIN_IS_SYNTH is true.
 */
#define DISTRHO_PLUGIN_WANT_MIDI_INPUT 0

/**
   Wherever the plugin wants MIDI output.
   @see Plugin::writeMidiEvent(const MidiEvent&)
 */
#define DISTRHO_PLUGIN_WANT_MIDI_OUTPUT 0

/**
   Wherever the plugin provides its own internal programs.
   @see Plugin::initProgramName(uint32_t, String&)
   @see Plugin::loadProgram(uint32_t)
 */
#define DISTRHO_PLUGIN_WANT_PROGRAMS 0

/**
   Wherever the plugin uses internal non-parameter data.
   @see Plugin::initState(uint32_t, String&, String&)
   @see Plugin::setState(const char*, const char*)
 */
#define DISTRHO_PLUGIN_WANT_STATE 0

/**
   Wherever the plugin wants time position information from the host.
   @see Plugin::getTimePosition()
 */
#define DISTRHO_PLUGIN_WANT_TIMEPOS 0

/**
   Wherever the %UI uses NanoVG for drawing instead of the default raw OpenGL calls.@n
   When enabled your %UI instance will subclass @ref NanoWidget instead of @ref Widget.
 */
#define DISTRHO_UI_USE_NANOVG 0

/**
   The %UI URI when exporting in LV2 format.@n
   By default this is set to @ref DISTRHO_PLUGIN_URI with "#UI" as suffix.
 */
#define DISTRHO_UI_URI DISTRHO_PLUGIN_URI "#UI"

/**
   Wherever the %UI uses a custom toolkit implementation based on OpenGL.@n
   When enabled, the macros @ref DISTRHO_UI_CUSTOM_INCLUDE_PATH and @ref DISTRHO_UI_CUSTOM_WIDGET_TYPE are required.
 */
#define DISTRHO_UI_USE_CUSTOM 1

/**
   The include path to the header file used by the custom toolkit implementation.
   This path must be relative to dpf/distrho/DistrhoUI.hpp
   @see DISTRHO_UI_USE_CUSTOM
 */
#define DISTRHO_UI_CUSTOM_INCLUDE_PATH "DearImGui.hpp"

/**
   The top-level-widget typedef to use for the custom toolkit.
   This widget class MUST be a subclass of DGL TopLevelWindow class.
   It is recommended that you keep this widget class inside the DGL namespace,
   and define widget type as e.g. DGL_NAMESPACE::MyCustomTopLevelWidget.
   @see DISTRHO_UI_USE_CUSTOM
 */
#define DISTRHO_UI_CUSTOM_WIDGET_TYPE DGL_NAMESPACE::ImGuiTopLevelWidget

#define DISTRHO_UI_USER_RESIZABLE   1
