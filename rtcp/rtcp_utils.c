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

/** @file rtcp_utils.c
 * This file contains the utility functions for RTCP Layer
 */

#include "rtcp.h"

#define RTCP_MIN_TIME 5.0
#define RTCP_SENDER_BW_FRACTION 0.25
#define RTCP_RCVR_BW_FRACTION 0.75
#define COMPENSATION 1.21828    /* e - 1.5 */

/**
 * Calculates an interval between rtcp reports 
 * @param members Number of members active in the session
 * @param senders Number of senders active in the session
 * @param rtcp_bw ?
 * @param we_sent TRUE -> Calculate interval for sending, FALSE -> for receiving
 * @param avg_rtcp_size ?
 * @param initial If it's the first interval we are calculating
 * @return Result time in ms
 */
double rtcp_interval(int members, int senders, double rtcp_bw, int we_sent,
             double avg_rtcp_size, int initial)
{
    double t;
    double rtcp_min_time = RTCP_MIN_TIME;
    int n;

    if (initial)
        rtcp_min_time /= 2;

    n = members;
    if (senders > 0 && senders < members * RTCP_SENDER_BW_FRACTION) {
        if (we_sent) {
            rtcp_bw *= RTCP_SENDER_BW_FRACTION;
            n = senders;
        } else {
            rtcp_bw *= RTCP_RCVR_BW_FRACTION;
            n -= senders;
        }
    }
    if ((t = avg_rtcp_size * n / rtcp_bw) < rtcp_min_time)
        t = rtcp_min_time;
    t = (t * (drand48() + 0.5)) / COMPENSATION;
    return t;
}
