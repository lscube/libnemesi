/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifndef __UTILS_H
#define __UTILS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/time.h>

#include <nemesi/comm.h>
#include <nemesi/types.h>

#define min(x,y) ((x) < (y) ? (x) : (y))
#define max(x,y) ((x) > (y) ? (x) : (y))

int urltokenize(char *, char **, char **, char **);
char *strstrcase(char *, const char *);
int strncmpcase(const char *, const char *, size_t);
int strcmpcase(const char *, const char *);
#if 0
char *strdup(const char *s);
#endif                /* !HAVE_STRDUP */
uint32 random32(int);
int timeval_subtract(struct timeval *, const struct timeval *,
             const struct timeval *);
int timeval_add(struct timeval *, const struct timeval *, const struct timeval *);
void f2time(double, struct timeval *);


#endif
