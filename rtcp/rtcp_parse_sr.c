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

int rtcp_parse_sr(rtp_ssrc * stm_src, rtcp_pkt * pkt)
{
    nms_printf(NMSML_DBG1, "Received SR from SSRC: %u\n", pkt->r.sr.ssrc);
    gettimeofday(&(stm_src->ssrc_stats.lastsr), NULL);
    stm_src->ssrc_stats.ntplastsr[0] = ntohl(pkt->r.sr.si.ntp_seq);
    stm_src->ssrc_stats.ntplastsr[1] = ntohl(pkt->r.sr.si.ntp_frac);
    /* Per ora, non ci interessa altro. */
    /* Forse le altre informazioni possono */
    /* servire per un monitor RTP/RTCP */
    return 0;
}
