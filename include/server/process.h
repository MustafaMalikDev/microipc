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

#ifndef _MIPC_SERVER_PROCESS_H_
#define _MIPC_SERVER_PROCESS_H_

#define MIPC_EMPTY_PROCESS() (struct mipc_process_request_t){.message = 0, .recv = 0, .pid = 0, .port = 0}

struct mipc_process_request_t {
    char message[255]; /* send()/write() */
    char recv[255];    /* recv()/read() */
    unsigned int pid;
    unsigned int port;
};

/* because it's two-way communication, there is no need to label one as the server or client */
struct mipc_process_mailbox_t {
    struct mipc_process_request_t first;
    struct mipc_process_request_t second;
};

int mipc_process_mailbox_empty(const struct mipc_process_mailbox_t);

int mipc_process_is_empty(const struct mipc_process_request_t);

struct mipc_process_request_t mipc_process_deserialise(const char*);

#endif /* _MIPC_SERVER_PROCESS_H_ */
