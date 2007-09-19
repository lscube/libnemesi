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

int rtcp_hdr_val_chk(rtcp_pkt * pkt, int len)
{
    rtcp_pkt *end;

    if (len < (int) sizeof(rtcp_common_t)) {
        nms_printf(NMSML_ERR, "RTCP packet too small!!! (%d)\n", len);
        return 1;
    }

    if (len / 4 > (ntohs((pkt->common).len) + 1)) {
        /* This is a fu**ing compound pkt */
        nms_printf(NMSML_DBG2,
               "RTCP Compound packet arrived (total len=%d)\n",
               len);
        if ((*(uint16_t *) pkt & RTCP_VALID_MASK) != RTCP_VALID_VALUE) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: first pkt of Compound is not a SR (or RR)!\n"
                   BLANK_LINE);
            return 1;
        }
        end = (rtcp_pkt *) ((uint32_t *) pkt + len / 4);
        do
            pkt =
                (rtcp_pkt *) ((uint32_t *) pkt +
                      ntohs((pkt->common).len) + 1);
        while ((pkt < end) && ((pkt->common).ver == 2));

        if (pkt != end) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching lenght (%d)!\n"
                   BLANK_LINE, len);
            return 1;
        }
    } else {
        nms_printf(NMSML_DBG2, "RTCP packet arrived (total len=%d)\n",
               len);
        if ((pkt->common).ver != RTP_VERSION) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching RTP version number!\n"
                   BLANK_LINE);
            return 1;
        }
        if (!(((pkt->common).pt >= 200) && ((pkt->common).pt <= 204))) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching payload type!\n"
                   BLANK_LINE);
            return 1;
        }
        if (((pkt->common).pad)
            && (*(((uint8_t *) pkt) + len - 1) > (pkt->common).len * 4)) {
            nms_printf(NMSML_WARN,
                   "RTCP Header not valid: mismatching lenght!\n"
                   BLANK_LINE);
            return 1;
        }
    }

    return 0;
}
