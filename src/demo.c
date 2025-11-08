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

#include "config.h"
#include "server/process.h"
#include "server/socket.h"

int main(void) {
    signal(SIGINT, mipc_socket_stop);

    // struct mipc_process_request_t res = mipc_process_deserialise("{.message=hello world,.pid=69420,.port=8080}");
    // println(res.message);
    // printf("%d\n", res.pid);
    // printf("%d\n", res.port);

    int res = mipc_socket_create("/tmp/mipc.sock", 255);

    if (!res) {
        printerr("(1) failed to create socket");
        exit(EXIT_FAILURE);
    }

    res = mipc_socket_start();

    if (!res) {
        printerr("(2) failed to create socket");
        exit(EXIT_FAILURE);
    }
}
