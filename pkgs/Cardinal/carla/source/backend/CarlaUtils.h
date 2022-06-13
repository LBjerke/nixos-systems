﻿/*
 * Carla Plugin Host
 * Copyright (C) 2011-2022 Filipe Coelho <falktx@falktx.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * For a full copy of the GNU General Public License see the doc/GPL.txt file.
 */

#ifndef CARLA_UTILS_H_INCLUDED
#define CARLA_UTILS_H_INCLUDED

#include "CarlaBackend.h"

#ifdef __cplusplus
using CarlaBackend::PluginCategory;
using CarlaBackend::PluginType;
#endif

/*!
 * @defgroup CarlaUtilsAPI Carla Utils API
 *
 * The Carla Utils API.
 *
 * This API allows to call advanced features from Python.
 * @{
 */

/*!
 * TODO.
 */
typedef void* CarlaPipeClientHandle;

/*!
 * TODO.
 */
typedef void (*CarlaPipeCallbackFunc)(void* ptr, const char* msg);

/*!
 * Information about a cached plugin.
 * @see carla_get_cached_plugin_info()
 */
typedef struct _CarlaCachedPluginInfo {
    /*!
     * Wherever the data in this struct is valid.
     * For performance reasons, plugins are only checked on request,
     *  and as such, the count vs number of really valid plugins might not match.
     * Use this field to skip on plugins which cannot be loaded in Carla.
     */
    bool valid;

    /*!
     * Plugin category.
     */
    PluginCategory category;

    /*!
     * Plugin hints.
     * @see PluginHints
     */
    uint hints;

    /*!
     * Number of audio inputs.
     */
    uint32_t audioIns;

    /*!
     * Number of audio outputs.
     */
    uint32_t audioOuts;

    /*!
     * Number of CV inputs.
     */
    uint32_t cvIns;

    /*!
     * Number of CV outputs.
     */
    uint32_t cvOuts;

    /*!
     * Number of MIDI inputs.
     */
    uint32_t midiIns;

    /*!
     * Number of MIDI outputs.
     */
    uint32_t midiOuts;

    /*!
     * Number of input parameters.
     */
    uint32_t parameterIns;

    /*!
     * Number of output parameters.
     */
    uint32_t parameterOuts;

    /*!
     * Plugin name.
     */
    const char* name;

    /*!
     * Plugin label.
     */
    const char* label;

    /*!
     * Plugin author/maker.
     */
    const char* maker;

    /*!
     * Plugin copyright/license.
     */
    const char* copyright;

#ifdef __cplusplus
    /*!
     * C++ constructor.
     */
    CARLA_API _CarlaCachedPluginInfo() noexcept;
    CARLA_DECLARE_NON_COPY_STRUCT(_CarlaCachedPluginInfo)
#endif

} CarlaCachedPluginInfo;

/* --------------------------------------------------------------------------------------------------------------------
 * cached plugins */

/*!
 * Get how many cached plugins are available.
 * Internal and LV2 plugin formats are cached and need to be discovered via this function.
 * Do not call this for any other plugin formats.
 *
 * @note if this carla build uses JUCE, then you must call carla_juce_init beforehand
 * @note for AU plugins, you cannot call this outside the main thread
 */
CARLA_EXPORT uint carla_get_cached_plugin_count(PluginType ptype, const char* pluginPath);

/*!
 * Get information about a cached plugin.
 *
 * @note if this carla build uses JUCE, then you must call carla_juce_init beforehand
 * @note for AU plugins, you cannot call this outside the main thread
 */
CARLA_EXPORT const CarlaCachedPluginInfo* carla_get_cached_plugin_info(PluginType ptype, uint index);

#ifndef CARLA_HOST_H_INCLUDED
/* --------------------------------------------------------------------------------------------------------------------
 * information */

/*!
 * Get the complete license text of used third-party code and features.
 * Returned string is in basic html format.
 */
CARLA_EXPORT const char* carla_get_complete_license_text(void);

/*!
 * Get the juce version used in the current Carla build.
 */
CARLA_EXPORT const char* carla_get_juce_version(void);

/*!
 * Get the list of supported file extensions in carla_load_file().
 */
CARLA_EXPORT const char* const* carla_get_supported_file_extensions(void);

/*!
 * Get the list of supported features in the current Carla build.
 */
CARLA_EXPORT const char* const* carla_get_supported_features(void);

/*!
 * Get the absolute filename of this carla library.
 */
CARLA_EXPORT const char* carla_get_library_filename(void);

/*!
 * Get the folder where this carla library resides.
 */
CARLA_EXPORT const char* carla_get_library_folder(void);
#endif

/* --------------------------------------------------------------------------------------------------------------------
 * JUCE */

/*!
 * Initialize data structures and GUI support for JUCE.
 * This is only needed when carla builds use JUCE and you call cached-plugin related APIs.
 *
 * Idle must then be called at somewhat regular intervals, though in practice there is no reason for it yet.
 *
 * Make sure to call carla_juce_cleanup after you are done with APIs that need JUCE.
 */
CARLA_EXPORT void carla_juce_init(void);

/*!
 * Give idle time to JUCE stuff.
 * Currently only used for Linux.
 */
CARLA_EXPORT void carla_juce_idle(void);

/*!
 * Cleanup the JUCE stuff that was initialized by carla_juce_init.
 */
CARLA_EXPORT void carla_juce_cleanup(void);

/* --------------------------------------------------------------------------------------------------------------------
 * pipes */

/*!
 * TODO.
 */
CARLA_EXPORT CarlaPipeClientHandle carla_pipe_client_new(const char* argv[], CarlaPipeCallbackFunc callbackFunc, void* callbackPtr);

/*!
 * TODO.
 */
CARLA_EXPORT void carla_pipe_client_idle(CarlaPipeClientHandle handle);

/*!
 * TODO.
 */
CARLA_EXPORT bool carla_pipe_client_is_running(CarlaPipeClientHandle handle);

/*!
 * TODO.
 */
CARLA_EXPORT void carla_pipe_client_lock(CarlaPipeClientHandle handle);

/*!
 * TODO.
 */
CARLA_EXPORT void carla_pipe_client_unlock(CarlaPipeClientHandle handle);

/*!
 * TODO.
 */
CARLA_EXPORT const char* carla_pipe_client_readlineblock(CarlaPipeClientHandle handle, uint timeout);

/*!
 * Extras.
 * TODO.
 */
CARLA_EXPORT bool carla_pipe_client_readlineblock_bool(CarlaPipeClientHandle handle, uint timeout);
CARLA_EXPORT int carla_pipe_client_readlineblock_int(CarlaPipeClientHandle handle, uint timeout);
CARLA_EXPORT double carla_pipe_client_readlineblock_float(CarlaPipeClientHandle handle, uint timeout);

/*!
 * TODO.
 */
CARLA_EXPORT bool carla_pipe_client_write_msg(CarlaPipeClientHandle handle, const char* msg);

/*!
 * TODO.
 */
CARLA_EXPORT bool carla_pipe_client_write_and_fix_msg(CarlaPipeClientHandle handle, const char* msg);

/*!
 * TODO.
 */
CARLA_EXPORT bool carla_pipe_client_flush(CarlaPipeClientHandle handle);

/*!
 * TODO.
 */
CARLA_EXPORT bool carla_pipe_client_flush_and_unlock(CarlaPipeClientHandle handle);

/*!
 * TODO.
 */
CARLA_EXPORT void carla_pipe_client_destroy(CarlaPipeClientHandle handle);

/* --------------------------------------------------------------------------------------------------------------------
 * system stuff */

/*!
 * Flush stdout or stderr.
 */
CARLA_EXPORT void carla_fflush(bool err);

/*!
 * Print the string @a string to stdout or stderr.
 */
CARLA_EXPORT void carla_fputs(bool err, const char* string);

/*!
 * Set the current process name to @a name.
 */
CARLA_EXPORT void carla_set_process_name(const char* name);

/* --------------------------------------------------------------------------------------------------------------------
 * window control */

/*!
 * Get the global/desktop scale factor.
 */
CARLA_EXPORT double carla_get_desktop_scale_factor(void);

CARLA_EXPORT int carla_cocoa_get_window(void* nsViewPtr);
CARLA_EXPORT void carla_cocoa_set_transient_window_for(void* nsViewChild, void* nsViewParent);

CARLA_EXPORT void carla_x11_reparent_window(uintptr_t winId1, uintptr_t winId2);

CARLA_EXPORT void carla_x11_move_window(uintptr_t winId, int x, int y);

CARLA_EXPORT int* carla_x11_get_window_pos(uintptr_t winId);

/* ----------------------------------------------------------------------------------------------------------------- */

/** @} */

#endif /* CARLA_UTILS_H_INCLUDED */
