/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
#if !HAVE_STRDUP
char *strdup(const char *s);
#endif                /* !HAVE_STRDUP */
uint32 random32(int);
int timeval_subtract(struct timeval *, const struct timeval *,
             const struct timeval *);
int timeval_add(struct timeval *, const struct timeval *,
        const struct timeval *);
void f2time(double, struct timeval *);


#endif
