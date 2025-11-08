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

#include "server/process.h"

#include "config.h"
#include "strutil.h"

static size_t g_mipc_process_extract(char dest[], char str[], char delim, size_t start) {
    size_t len = strlen(str);
    size_t index = start;
    size_t dest_index = 0;

    memset(dest, 0, len);

    while (str[index] != delim) {
        if (dest_index >= len) {
            break;
        }

        dest[dest_index] = str[index];
        dest_index++;
        index++;
    }

    return index;
}

int mipc_process_mailbox_empty(const struct mipc_process_mailbox_t mailbox) {
    return !mailbox.first.port;
}

int mipc_process_is_empty(const struct mipc_process_request_t process) {
    return process.pid == 0 && process.port == 0;
}

/* {.message=MESSAGE,.pid=0,.port=0} */
struct mipc_process_request_t mipc_process_deserialise(const char* request) {
    if (!request) {
        printerr("invalid request body");
        return MIPC_EMPTY_PROCESS();
    }

    struct mipc_process_request_t data = MIPC_EMPTY_PROCESS();
    size_t len = strlen(request);

    char req_copy[len];
    char buffer[len];
    char message[255];

    uint32_t pid = 0;
    uint32_t port = 0;

    memset(req_copy, 0, len);
    memset(buffer, 0, len);
    memset(message, 0, 255);
    strcpy(req_copy, request);

    char first = req_copy[0];
    char last = req_copy[len - 1];

    if (first != '{' || last != '}') {
        printerr("invalid brace syntax");
        return MIPC_EMPTY_PROCESS();
    }

    size_t next_index = g_mipc_process_extract(buffer, req_copy, ',', 2);
    strcpy(message, strremove(buffer, "message="));

    next_index = g_mipc_process_extract(buffer, req_copy, ',', next_index + 2);
    strremove(buffer, "pid=");
    pid = atoi(buffer);

    g_mipc_process_extract(buffer, req_copy, ',', next_index + 2);
    strremove(buffer, "port=");
    port = atoi(buffer);

    memset(data.message, 0, 255);
    strcpy(data.message, message);

    data.pid = pid;
    data.port = port;

    return data;
}
