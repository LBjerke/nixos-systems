/*
 * Carla Main-Loop utils
 * Copyright (C) 2017 Filipe Coelho <falktx@falktx.com>
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

#ifndef CARLA_MAIN_LOOP_HPP_INCLUDED
#define CARLA_MAIN_LOOP_HPP_INCLUDED

#include "CarlaBackend.h"
#include "CarlaUtils.hpp"

#ifdef CARLA_OS_MAC
# import <Cocoa/Cocoa.h>
#endif

// ---------------------------------------------------------------------------------------------------------------------

CARLA_BACKEND_START_NAMESPACE

static inline
bool runMainLoopOnce()
{
#if defined(CARLA_OS_MAC)
    NSAutoreleasePool* const pool = [[NSAutoreleasePool alloc] init];
    NSDate* const date = [NSDate distantPast];
    NSEvent* event;

    for (;;)
    {
        event = [NSApp
                 nextEventMatchingMask:NSAnyEventMask
                             untilDate:date
                                inMode:NSDefaultRunLoopMode
                               dequeue:YES];

        if (event == nil)
            break;

        [NSApp sendEvent: event];
    }

    [pool release];
#elif defined(CARLA_OS_WIN)
    MSG msg;
    if (! ::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
        return true;

    if (::GetMessage(&msg, nullptr, 0, 0) >= 0)
    {
        if (msg.message == WM_QUIT)
            return false;

        //TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
#endif

    return true;
}

CARLA_BACKEND_END_NAMESPACE

// ---------------------------------------------------------------------------------------------------------------------

#endif // CARLA_MAIN_LOOP_HPP_INCLUDED
