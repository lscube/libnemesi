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

/** @file rtcp_bye.c
 * This file contains the functions that perform BYE packet
 * handling and sending
 */

#include "nemesi/rtcp.h"
#include "nemesi/comm.h"
#include "rtsp/rtspinternals.h"

/**
 * BYE packet handler, when rtcp layer gets a bye packet
 * it signals it to the rtp layer reporting the end
 * of stream.
 * @param ssrc The SSRC for which the packet was received
 * @param pkt The packet itself
 * @return 0
 */
int rtcp_parse_bye(rtp_ssrc * ssrc, rtcp_pkt * pkt)
{
    rtsp_thread * rtsp_t;
    int i;
    for (i = 0; i < pkt->common.count; i++)
        nms_printf(NMSML_DBG3, "Received BYE from SSRC: %u\n",
            pkt->r.bye.src[i]);

    rtsp_t = ssrc->rtp_sess->owner;
    rtsp_t->rtp_th->run = 0;
    return 0;
}

/**
 * Sends the Bye packet. Actually it does nothing
 * @param rtp_sess The session for which to send the bye packet
 * @return 0
 */
int rtcp_send_bye(rtp_session * rtp_sess)
{
    // TODO: really send bye packet
    nms_printf(NMSML_DBG1,
           "SRRC %d: sending RTCP Bye. Warning! Not yet implemented!",
           rtp_sess->local_ssrc);
    return 0;
}
