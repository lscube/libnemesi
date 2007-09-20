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

int rtcp_send_rr(rtp_session * rtp_sess)
{
    rtcp_pkt *pkt;
    int len;
    uint32_t rr_buff[MAX_PKT_SIZE];
    rtp_ssrc *stm_src;

    memset(rr_buff, 0, MAX_PKT_SIZE * sizeof(uint32_t));
    pkt = (rtcp_pkt *) rr_buff;

    len = rtcp_build_rr(rtp_sess, pkt);    /* in 32 bit words */
    pkt = (rtcp_pkt *) (rr_buff + len);
    len += rtcp_build_sdes(rtp_sess, pkt, (MAX_PKT_SIZE >> 2) - len);    /* in 32 bit words */

    for (stm_src = rtp_sess->ssrc_queue; stm_src; stm_src = stm_src->next)
        if ( !(stm_src->no_rtcp) && stm_src->rtp_sess->transport.RTCP.sock.fd > 0) {
            switch(stm_src->rtp_sess->transport.type) {
            case UDP:
                if (sendto(stm_src->rtp_sess->transport.RTCP.sock.fd,
                       rr_buff, (len << 2), 0, stm_src->rtcp_from.addr,
                       stm_src->rtcp_from.addr_len) < 0)
                    nms_printf(NMSML_WARN,
                           "WARNING! Error while sending UDP RTCP pkt\n");
                else
                    nms_printf(NMSML_DBG1,
                           "RTCP RR packet sent\n");
                break;
            case SCTP:
            case TCP:
                if (send(stm_src->rtp_sess->transport.RTCP.sock.fd,
                       rr_buff, (len << 2), 0) < 0)
                    nms_printf(NMSML_WARN,
                           "WARNING! Error while sending local RTCP pkt\n");
                else
                    nms_printf(NMSML_DBG1,
                           "RTCP RR packet sent\n");
                break;
            }
        }

    return len;
}
