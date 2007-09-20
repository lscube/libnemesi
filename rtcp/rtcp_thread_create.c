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

#include <nemesi/rtcp.h>

int rtcp_thread_create(rtp_thread * rtp_th)
{
    int n;
    pthread_attr_t rtcp_attr;

    pthread_attr_init(&rtcp_attr);
    if (pthread_attr_setdetachstate(&rtcp_attr, PTHREAD_CREATE_JOINABLE) !=
        0)
        return nms_printf(NMSML_FATAL,
                  "Cannot set RTCP Thread attributes!\n");

    if ((n =
         pthread_create(&rtp_th->rtcp_tid, &rtcp_attr, &rtcp,
                (void *) rtp_th)) > 0)
        return nms_printf(NMSML_FATAL, "%s\n", strerror(n));

    return 0;
}
