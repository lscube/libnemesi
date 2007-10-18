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

struct rtcp_event *rtcp_handle_event(struct rtcp_event *event)
{

    double t;
    struct timeval tv, now;
    rtp_session *rtp_save;
    int n;

    gettimeofday(&now, NULL);

    switch (event->type) {

    case RTCP_RR:
    case RTCP_SDES:

        if (event->rtp_sess->ssrc_queue) {
            n = rtcp_send_rr(event->rtp_sess);
            event->rtp_sess->sess_stats.avg_rtcp_size =
                (1. / 16.) * n +
                (15. / 16.) *
                (event->rtp_sess->sess_stats.avg_rtcp_size);
        }
        event->rtp_sess->sess_stats.tp = now;

        t = rtcp_interval(event->rtp_sess->sess_stats.members,
                  event->rtp_sess->sess_stats.senders,
                  event->rtp_sess->sess_stats.rtcp_bw,
                  event->rtp_sess->sess_stats.we_sent,
                  event->rtp_sess->sess_stats.avg_rtcp_size,
                  event->rtp_sess->sess_stats.initial);

        tv.tv_sec = (long int) t;
        tv.tv_usec = (long int) ((t - tv.tv_sec) * 1000000);
        nms_timeval_add(&(event->rtp_sess->sess_stats.tn), &now, &tv);

        event->rtp_sess->sess_stats.initial = 0;
        event->rtp_sess->sess_stats.pmembers =
            event->rtp_sess->sess_stats.members;

        rtp_save = event->rtp_sess;
        event = rtcp_deschedule(event);
        if ((event =
             rtcp_schedule(event, rtp_save, rtp_save->sess_stats.tn,
                   RTCP_RR)) == NULL)
            return NULL;

        break;

    case RTCP_BYE:
        rtcp_send_bye(event->rtp_sess);
        break;
    default:
        nms_printf(NMSML_ERR, "RTCP Event not handled!\n");
        break;
    }
    return event;
}
