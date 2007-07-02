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

int rtp_thread_create(rtp_thread * rtp_th)
{
    int n;
    pthread_attr_t rtp_attr;
    rtp_session *rtp_sess;
    rtp_fmts_list *fmt;

    pthread_attr_init(&rtp_attr);
    if (pthread_attr_setdetachstate(&rtp_attr, PTHREAD_CREATE_JOINABLE) !=
        0)
        return nms_printf(NMSML_FATAL,
                  "Cannot set RTP Thread attributes (detach state)\n");

    if ((n =
         pthread_create(&rtp_th->rtp_tid, &rtp_attr, &rtp,
                (void *) rtp_th)) > 0)
        return nms_printf(NMSML_FATAL, "%s\n", strerror(n));

    for (rtp_sess = rtp_th->rtp_sess_head; rtp_sess;
         rtp_sess = rtp_sess->next) {
        for (fmt = rtp_sess->announced_fmts; fmt; fmt = fmt->next)
            if (rtp_sess->parsers_inits[fmt->pt])
                rtp_sess->parsers_inits[fmt->pt] (rtp_sess,
                                  fmt->pt);
    }

    rtp_th->run = 1;
    return 0;
}
