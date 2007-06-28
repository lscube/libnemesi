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

#include <nemesi/rtcp.h>

int rtcp_parse_pkt(rtp_ssrc * stm_src, rtcp_pkt * pkt, int len)
{
    rtcp_pkt *end;
    end = (rtcp_pkt *) ((uint32 *) pkt + len / 4);

    while (pkt < end) {
        switch ((pkt->common).pt) {
        case RTCP_SR:
            rtcp_parse_sr(stm_src, pkt);
            break;
        case RTCP_SDES:
            if (rtcp_parse_sdes(stm_src, pkt))
                return -1;
            break;
        case RTCP_RR:
            rtcp_parse_rr(pkt);
            break;
        case RTCP_BYE:
            rtcp_parse_bye(pkt);
            break;
        case RTCP_APP:
            rtcp_parse_app(pkt);
            break;
        default:
            nms_printf(NMSML_WARN, "Received unknown RTCP pkt\n");
            return 1;
        }
        pkt =
            (rtcp_pkt *) ((uint32 *) pkt + ntohs((pkt->common).len) +
                  1);
    }
    return 0;
}
