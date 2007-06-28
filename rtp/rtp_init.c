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

#include <nemesi/rtp.h>
#include <nemesi/comm.h>

rtp_thread *rtp_init(void)
{
    rtp_thread *rtp_th;

    if (!(rtp_th = (rtp_thread *) calloc(1, sizeof(rtp_thread)))) {
        nms_printf(NMSML_FATAL, "Could not alloc memory!\n");
        return NULL;
    }

    rtp_parsers_init();

    if (pthread_mutex_init(&(rtp_th->syn), NULL)) {
        free(rtp_th);
        return NULL;
    }
    /* Decoder blocked 'till buffering is complete */
    pthread_mutex_lock(&(rtp_th->syn));

    return rtp_th;
}
