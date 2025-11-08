/**
 * This file is part of the microipc distribution (https://github.com/MustafaMalikDev/microipc)
 * Copyright (c) 2025 Mustafa Malik.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#define MIPC_USE_STD

#include "server/dispatch.h"
#include "server/table.h"

#include "config.h"
#include <string.h>

int mipc_dispatch_send_msg(int port, int pid, const char* msg) {
    if (!port && !pid) {
        printerr("invalid port or pid for dispatch");
        return FALSE;
    }

    if (!msg) {
        printerr("invalid message for dispatch");
        return FALSE;
    }

    struct mipc_process_mailbox_t* server = mipc_table_get_mailbox(port, pid);

    if (!server) {
        printerr("no server found from mailbox for dispatch");
        return FALSE;
    }

    memset(server->first.message, 0, 255);
    strcpy(server->first.message, msg);

    /* now send a nice response back to the client :) */
    char response[255];

    memset(server->second.message, 0, 255);
    memset(response, 0, 255);

    snprintf(response, 255, "response written to port: %d", server->first.port);
    strcpy(server->second.message, response);

    return TRUE;
}
