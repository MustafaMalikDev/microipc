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

#define MIPC_USE_LIBS
#define MIPC_USE_STD

#include "server/socket.h"
#include "server/dispatch.h"
#include "server/process.h"
#include "server/table.h"

#include "config.h"
#include "strutil.h"

#include <string.h>
#include <sys/event.h>

static char* g_socket_name;
static int g_buffer_size;
static int g_ready = FALSE;
static int g_running = FALSE;
static int g_socket = -1;

int mipc_socket_create(const char* name, int size) {
    if (!name || size <= 0 || g_ready || g_running) {
        return FALSE;
    }

    if (strlen(name) > 103) {
        printerr("name too long");
        return FALSE;
    }

    g_socket_name = (char*)name;
    g_buffer_size = size;
    g_ready = TRUE;

    /* if the socket already existed */
    unlink(name);
    return TRUE;
}

int mipc_socket_start(void) {
    if (!g_ready || g_running) {
        return FALSE;
    }

    char buffer[MIPC_MAX_POLL_FDS][g_buffer_size];
    int kq = kqueue();
    int next_ev = -1;
    int data = 0;
    int result = -1;

    struct kevent event;
    struct sockaddr_un name;

    for (int i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        memset(buffer[i], 0, sizeof(buffer[i]));
    }

    g_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (g_socket == -1) {
        err("could not create server socket");
        return FALSE;
    }

    EV_SET(&event, g_socket, EVFILT_READ, EV_ADD, 0, 0, NULL);
    if (kevent(kq, &event, 1, NULL, 0, NULL) == -1) {
        err("couldn't configure kernel queue");
        close(g_socket);
        return FALSE;
    }

    memset(&name, 0, sizeof(struct sockaddr_un));

    name.sun_family = AF_UNIX;
    name.sun_len = MIPC_SUN_SOCK_LEN + 1;
    strncpy(name.sun_path, g_socket_name, MIPC_SUN_SOCK_LEN);

    result = bind(g_socket, (const struct sockaddr*)&name, sizeof(struct sockaddr_un));

    if (result == -1) {
        err("could not bind the process to socket");
        mipc_socket_stop(SIGTERM);
        return FALSE;
    }

    if (listen(g_socket, MIPC_MAX_POLL_FDS) == -1) {
        err("could not listen");
        mipc_socket_stop(SIGTERM);
        return FALSE;
    }

    g_running = TRUE;
    result = 0;

    struct kevent eset;
    struct kevent events[MIPC_MAX_POLL_FDS];
    int clients[MIPC_MAX_POLL_FDS];
    int fd;

    memset(clients, -1, MIPC_MAX_POLL_FDS);
    while (g_running) {
        next_ev = kevent(kq, NULL, 0, events, MIPC_MAX_POLL_FDS, NULL);

        if (next_ev < 1) {
            err("failed to read kernel event in loop");
        }

        for (int i = 0; i < next_ev; i++) {
            if (events[i].flags & EV_EOF) {
                println("client disconnect request acknowledged");

                fd = events[i].ident;
                EV_SET(&eset, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);

                if (kevent(kq, &eset, 1, NULL, 0, NULL) == -1) {
                    err("could not disconnect client");
                    continue;
                }

                memset(buffer[i], 0, sizeof(buffer[i]));
                clients[i] = -1;
                close(fd);
            } else if (events[i].ident == (uintptr_t)g_socket) {
                fd = accept(events[i].ident, NULL, NULL);

                if (fd == -1) {
                    err("failed to accept connection from client");
                    continue;
                }

                clients[i] = fd;
                EV_SET(&eset, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);

                if (kevent(kq, &eset, 1, NULL, 0, NULL) == -1) {
                    err("could not handle new client connection");
                }
            } else if (events[i].filter == EVFILT_READ) {
                data = recv(events[i].ident, buffer[i], sizeof(buffer[i]), 0);
                if (data > 0) {
                    const char* message = strtrim(buffer[i]);
                    const char* copy = message;
                    struct mipc_process_request_t request;

                    if (*message == 'c') {
                        request = mipc_process_deserialise(strtrim((char*)++copy));
                        mipc_table_insert(request);
                    }

                    if (*message == 'r') {
                        request = mipc_process_deserialise(strtrim((char*)++copy));
                        mipc_table_remove(request);
                        mipc_table_destroy_queue(request);
                        mipc_table_print_queue();
                    }

                    if (*message == '{') {
                        request = mipc_process_deserialise(strtrim((char*)message));

                        struct mipc_process_request_t target = MIPC_EMPTY_PROCESS();
                        target.port = request.port;

                        int port = target.port;
                        int pid = request.pid;

                        if (mipc_table_contains(target) > -1) {
                            /*
                                that means a request to an existing port exists,
                                so we create a new mailbox queue with the target
                            */
                            if (mipc_table_queue_contains_both(port, pid) == -1) {
                                mipc_table_shift_to_queue(target);
                                mipc_table_map_to_queue(request);
                                mipc_table_print_queue();
                            } else {
                                /* if they exist and are mapped, let's send some messages */
                                if (mipc_dispatch_send_msg(port, pid, request.message)) {
                                    struct mipc_process_mailbox_t* mailbox = mipc_table_get_mailbox(port, pid);
                                    if (mailbox) {
                                        println(mailbox->first.message);
                                        write(events[i].ident, mailbox->second.message, 255);
                                    }
                                }
                            }
                        }
                    }

                    memset(buffer[i], 0, sizeof(buffer[i]));
                }
            }
        }
    }

    mipc_socket_stop(SIGTERM);
    return TRUE;
}

void mipc_socket_stop(int __attribute__((unused)) _) {
    unlink(g_socket_name);
    close(g_socket);

    g_running = FALSE;
    g_ready = FALSE;
}
