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

#ifndef _MIPC_SERVER_TABLE_H_
#define _MIPC_SERVER_TABLE_H_

#define MIPC_USE_STD

#include "config.h"
#include "process.h"

struct mipc_table_process_entry {
    struct mipc_process_request_t process[MIPC_MAX_POLL_FDS];
    int8_t last : 3;
    uint8_t current : 3;
};

/*
    originally had multiple members, that's why it has it's own struct,
    can't be bothered to refactor this all
*/
struct mipc_table_mailbox_entry {
    struct mipc_process_mailbox_t queue[MIPC_MAX_POLL_FDS];
};

struct mipc_table_t {
    struct mipc_table_process_entry proc_entry;
    struct mipc_table_mailbox_entry mail_entry;
};

int8_t mipc_table_contains(const struct mipc_process_request_t);

void mipc_table_insert(const struct mipc_process_request_t);

void mipc_table_update(const struct mipc_process_request_t);

void mipc_table_remove(const struct mipc_process_request_t);

struct mipc_process_mailbox_t* mipc_table_get_mailbox(int, int);

int8_t mipc_table_queue_contains(const struct mipc_process_request_t);

int8_t mipc_table_queue_contains_both(uint32_t, uint32_t);

void mipc_table_shift_to_queue(const struct mipc_process_request_t);

void mipc_table_map_to_queue(const struct mipc_process_request_t);

void mipc_table_destroy_queue(const struct mipc_process_request_t);

void mipc_table_print_queue(void);

#endif /* _MIPC_SERVER_TABLE_H_ */
