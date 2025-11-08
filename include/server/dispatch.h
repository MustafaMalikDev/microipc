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

#ifndef _MIPC_SERVER_DISPATCH_H_
#define _MIPC_SERVER_DISPATCH_H_

#include "process.h"

int mipc_dispatch_send_msg(int, int, const char*);

#endif /* _MIPC_SERVER_DISPATCH_H_ */
