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

//XXX get a saner implementation.

/**
 * @file timeval.c
 * timeval manipulation
 */
#include "utils.h"

/**
 * timeval addition
 * @param x first value
 * @param y second value
 * @param res sum of y and x
 */

int nms_timeval_add(struct timeval *res, const struct timeval *x,
        const struct timeval *y)
{
    res->tv_sec = x->tv_sec + y->tv_sec;

    res->tv_usec = x->tv_usec + y->tv_usec;

    if (res->tv_usec > 1000000) {
        res->tv_usec -= 1000000;
        res->tv_sec++;
    }

    return 0;
}

/**
 * timeval subtraction
 * @param x first value
 * @param y second value
 * @param res difference between x and y
 */

int nms_timeval_subtract(struct timeval *res, const struct timeval *x,
             const struct timeval *y)
{
    int nsec;
    struct timeval z = *y;

    /* Perform the carry for the later subtraction by updating Y. */
    if (x->tv_usec < z.tv_usec) {
        nsec = (z.tv_usec - x->tv_usec) / 1000000 + 1;
        z.tv_usec -= 1000000 * nsec;
        z.tv_sec += nsec;
    }
    if (x->tv_usec - z.tv_usec > 1000000) {
        nsec = (x->tv_usec - z.tv_usec) / 1000000;
        z.tv_usec += 1000000 * nsec;
        z.tv_sec -= nsec;
    }

    /* Compute the time remaining to wait.
     * `tv_usec' is certainly positive. */
    if (res != NULL) {
        res->tv_sec = x->tv_sec - z.tv_sec;
        res->tv_usec = x->tv_usec - z.tv_usec;
    }

    /* Return 1 if result is negative (x < y).  */
    return ((x->tv_sec < z.tv_sec)
        || ((x->tv_sec == z.tv_sec) && (x->tv_usec < z.tv_usec)));
}

/*
 * Convert a float value (seconds) to a timeval
 */

void f2time(double ftime, struct timeval *time)
{
    time->tv_sec = (long) ftime;
    time->tv_usec = (long) ((ftime - time->tv_sec) * 1000000);
}
