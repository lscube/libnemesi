/* * 
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

/**
 * @file utils.h
 * Miscellaneus private utility functions
 * Most of them should be inlined.
 */

#ifndef NEMESI_UTILS_H
#define NEMESI_UTILS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <inttypes.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/time.h>

#include "comm.h"

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

int urltokenize(char *, char **, char **, char **);

char *strstrcase(char *, const char *);

uint32_t random32(int);

/**
 * @defgroup timeval Timeval arithmetic functions
 *
 * @{
 */
int nms_timeval_subtract(struct timeval *, const struct timeval *,
             const struct timeval *);
int nms_timeval_add(struct timeval *, const struct timeval *, const struct timeval *);
void f2time(double, struct timeval *);
/**
 * @}
 */

#endif /* NEMESI_UTILS_H */
