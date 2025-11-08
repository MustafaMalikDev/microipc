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

#include "server/table.h"
#include <string.h>

static struct mipc_table_t g_table = {0};

struct mipc_process_mailbox_t* mipc_table_get_mailbox(int port, int pid) {
    int8_t entry = mipc_table_queue_contains_both(port, pid);

    if (entry == -1) {
        return NULL;
    }

    return &g_table.mail_entry.queue[entry];
}

int8_t mipc_table_contains(const struct mipc_process_request_t request) {
    if (mipc_process_is_empty(request)) {
        printerr("request is empty");
        return FALSE;
    }

    uint32_t port = request.port;

    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        /* clang-format off */
        if (
            g_table.proc_entry.process[i].port == port ||
            g_table.proc_entry.process[i].pid == request.pid
        ) {
            return i;
        }
        /* clang-format on */
    }

    return -1;
}

void mipc_table_insert(const struct mipc_process_request_t request) {
    if (mipc_table_contains(request) >= 0) {
        printerr("cannot insert same process in entry");
        return;
    }

    if (g_table.proc_entry.current >= MIPC_MAX_POLL_FDS) {
        printerr("maximum process table count reached");
        return;
    }

    struct mipc_table_process_entry* proc_table = &g_table.proc_entry;

    if (proc_table->last != -1) {
        proc_table->process[proc_table->last] = request;
        proc_table->last = -1;
    } else {
        proc_table->process[proc_table->current] = request;
    }

    proc_table->current++;
}

void mipc_table_update(const struct mipc_process_request_t request) {
    int8_t index = mipc_table_contains(request);

    if (index == -1) {
        printerr("cannot update process that doesn't exist in entry");
        return;
    }

    g_table.proc_entry.process[index] = request;
}

void mipc_table_remove(const struct mipc_process_request_t request) {
    int8_t index = mipc_table_contains(request);

    if (index == -1) {
        /* printerr("cannot remove process that doesn't exist in entry"); */
        return;
    }

    memset(&g_table.proc_entry.process[index], 0, sizeof(struct mipc_process_request_t));
    g_table.proc_entry.current--;
    g_table.proc_entry.last = (index != (MIPC_MAX_POLL_FDS - 1)) ? index : -1;
}

int8_t mipc_table_queue_contains(const struct mipc_process_request_t request) {
    struct mipc_table_mailbox_entry mail_entry = g_table.mail_entry;
    uint32_t port = request.port;

    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        struct mipc_process_mailbox_t queue = mail_entry.queue[i];

        if (!queue.first.port || !queue.second.pid) {
            continue;
        }

        if (queue.first.port == port || queue.second.pid == request.pid) {
            return TRUE;
        }
    }

    return FALSE;
}

int8_t mipc_table_queue_contains_both(uint32_t port, uint32_t pid) {
    if (!port || !pid) {
        return -1;
    }

    struct mipc_table_mailbox_entry mail_entry = g_table.mail_entry;

    for (int8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        struct mipc_process_mailbox_t queue = mail_entry.queue[i];
        if (queue.first.port == port && queue.second.pid == pid) {
            return i;
        }
    }

    return -1;
}

void mipc_table_shift_to_queue(const struct mipc_process_request_t request) {
    if (mipc_table_contains(request) == -1) {
        printerr("cannot shift to queue, must be in process table first");
        return;
    }

    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        struct mipc_process_request_t* first = &g_table.mail_entry.queue[i].first;
        if (!first || !first->port) {
            mipc_table_remove(request);
            *first = request;
            break;
        }
    }
}

void mipc_table_map_to_queue(const struct mipc_process_request_t client) {
    struct mipc_table_mailbox_entry* mail_entry = &g_table.mail_entry;
    uint32_t port = client.port;

    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        struct mipc_process_mailbox_t* queue = &mail_entry->queue[i];

        if (queue->first.port == port && !queue->second.pid) {
            queue->second = (struct mipc_process_request_t)client;
            break;
        }
    }
}

void mipc_table_destroy_queue(const struct mipc_process_request_t request) {
    if (!mipc_table_queue_contains(request)) {
        return;
    }

    struct mipc_table_mailbox_entry mail_entry = g_table.mail_entry;
    struct mipc_process_mailbox_t tmp[MIPC_MAX_POLL_FDS];

    uint32_t port = request.port;
    uint8_t tmp_idx = 0;

    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        struct mipc_process_request_t first = mail_entry.queue[i].first;
        struct mipc_process_request_t second = mail_entry.queue[i].second;

        if (first.port == port || second.pid == request.pid) {
            memset(&g_table.mail_entry.queue[i], 0, sizeof(struct mipc_process_mailbox_t));
        }
    }

    /* sorting */
    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        if (mail_entry.queue[i].first.port) {
            tmp[tmp_idx] = mail_entry.queue[i];
            tmp_idx++;
        }
    }

    memset(mail_entry.queue, 0, sizeof(struct mipc_table_mailbox_entry));
    memcpy(mail_entry.queue, tmp, tmp_idx);
    println("removed process from mailbox queue and destroyed all references");
}

void mipc_table_print_queue(void) {
    struct mipc_table_mailbox_entry mail_entry = g_table.mail_entry;

    for (uint8_t i = 0; i < MIPC_MAX_POLL_FDS; i++) {
        struct mipc_process_request_t first = mail_entry.queue[i].first;
        struct mipc_process_request_t second = mail_entry.queue[i].second;

        printf("first %d\n", first.port);
        printf("second %d\n", second.pid);
    }
}
