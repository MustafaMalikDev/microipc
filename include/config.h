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

#ifndef _MIPC_CONFIG_H_
#define _MIPC_CONFIG_H_

// clang-format off

#ifndef __APPLE__
#   error microipc only supports Apple devices
#endif

#include <TargetConditionals.h>

#if !defined(TARGET_OS_MAC) && !defined(TARGET_OS_OSX)
#   error micropic only supports macOS
#endif

#ifndef TARGET_CPU_ARM64
#   error microipc only supports AArch64 Architecture
#endif

#define TRUE 1
#define FALSE 0

#define MIPC_SUN_SOCK_LEN 103 /* minus the NULL terminator */
#define MIPC_MAX_POLL_FDS 6

#ifdef MIPC_USE_LIBS
#   include <unistd.h>
#   include <sys/socket.h>
#   include <sys/un.h>
#   include <sys/types.h>
#endif

#ifdef MIPC_USE_STD
#   include <stdlib.h>
#   include <stdio.h>

#   define println(msg) printf("%s\n", msg)
#   define printlnm(type, msg) printf("[%s]: %s\n", type, msg)
#   define printerr(msg) printf("[ERROR]: %s\n", msg)

#   define err(msg) perror(msg)
#   define panic(msg) \
        printerr(msg); \
        exit(-1);
#endif

// clang-format on

#endif /* _MIPC_CONFIG_H_ */
