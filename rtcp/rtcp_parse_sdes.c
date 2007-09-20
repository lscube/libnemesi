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

int rtcp_parse_sdes(rtp_ssrc * stm_src, rtcp_pkt * pkt)
{
    int8_t count = pkt->common.count;
    rtcp_sdes_t *sdes = &(pkt->r.sdes);
    rtcp_sdes_item_t *rsp, *rspn;
    rtcp_sdes_item_t *end =
        (rtcp_sdes_item_t *) ((uint32_t *) pkt + pkt->common.len + 1);

    nms_printf(NMSML_DBG1, "Received SDES from SSRC: %u\n",
           pkt->r.sdes.src);
    while (--count >= 0) {
        rsp = &(sdes->item[0]);
        if (rsp >= end)
            break;
        for (; rsp->type; rsp = rspn) {
            rspn =
                (rtcp_sdes_item_t *) ((uint8_t *) rsp + rsp->len +
                          2);
            if (rspn >= end) {
                rsp = rspn;
                break;
            }
            if (rtcp_set_ssrc_sdes(stm_src, rsp))
                return 1;
        }
        sdes =
            (rtcp_sdes_t *) ((uint32_t *) sdes +
                     (((uint8_t *) rsp - (uint8_t *) sdes) >> 2) +
                     1);
    }
    if (count >= 0)
        nms_printf(NMSML_WARN, "Invalid RTCP SDES pkt format!\n");
    else if (stm_src->ssrc_stats.probation)
        stm_src->ssrc_stats.probation = 1;
    return 0;
}
