/*
  ZynAddSubFX - a software synthesizer

  LASHClient.cpp - LASH support
  Copyright (C) 2006-2009 Lars Luthman
  Author: Lars Luthman

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.
*/
#include <unistd.h>
#include <iostream>
#include <string>

#include "LASHClient.h"

namespace zyncarla {

LASHClient::LASHClient(int *argc, char ***argv)
{
    client = lash_init(lash_extract_args(argc, argv), "ZynAddSubFX",
                       LASH_Config_File, LASH_PROTOCOL(2, 0));
}


void LASHClient::setalsaid(int id)
{
    if(lash_enabled(client))
        if(id != -1)
            lash_alsa_client_id(client, id);
}


void LASHClient::setjackname(const char *name)
{
    if(lash_enabled(client))
        if(name != NULL) {
            lash_jack_client_name(client, name);

            lash_event_t *event = lash_event_new_with_type(LASH_Client_Name);
            lash_event_set_string(event, name);
            lash_send_event(client, event);
        }
}


LASHClient::Event LASHClient::checkevents(std::string &filename)
{
    if(!lash_enabled(client))
        return NoEvent;

    Event received = NoEvent;
    lash_event_t *event;
    while((event = lash_get_event(client))) {
        // save
        if(lash_event_get_type(event) == LASH_Save_File) {
            std::cerr << "LASH event: LASH_Save_File" << std::endl;
            filename = std::string(lash_event_get_string(event))
                       + "/master.xmz";
            received = Save;
            break;
        }
        // restore
        else
        if(lash_event_get_type(event) == LASH_Restore_File) {
            std::cerr << "LASH event: LASH_Restore_File" << std::endl;
            filename = std::string(lash_event_get_string(event))
                       + "/master.xmz";
            received = Restore;
            break;
        }
        // quit
        else
        if(lash_event_get_type(event) == LASH_Quit) {
            std::cerr << "LASH event: LASH_Quit" << std::endl;
            received = Quit;
            break;
        }

        lash_event_destroy(event);
    }
    return received;
}


void LASHClient::confirmevent(Event event)
{
    if(event == Save)
        lash_send_event(client, lash_event_new_with_type(LASH_Save_File));
    else
    if(event == Restore)
        lash_send_event(client, lash_event_new_with_type(LASH_Restore_File));
}

}
