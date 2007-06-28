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

int rtp_hdr_val_chk(rtp_pkt * pkt, int len)
{
    if (RTP_PAYLOAD_SIZE(pkt, len) < 0) {
//      if (len < 12) {
        nms_printf(NMSML_ERR,
               "RTP packet too small (%d: smaller than RTP header size)!!!\n",
               len);
        return 1;
    }

    if (pkt->ver != RTP_VERSION) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching version number!"
               BLANK_LINE);
        return 1;
    }
    if ((pkt->pt >= 200) && (pkt->pt <= 204)) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching payload type!"
               BLANK_LINE);
        return 1;
    }
    if ((pkt->pad)
        && (*(((uint8 *) pkt) + len - 1) >
        (len - ((uint8 *) (pkt->data) - (uint8 *) pkt)))) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching lenght!"
               BLANK_LINE);
        return 1;
    }
    if ((pkt->cc)
        && (pkt->cc >
        (len - ((uint8 *) (pkt->data) - (uint8 *) pkt)) -
        ((*(((uint8 *) pkt) + len - 1)) * pkt->pad))) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching CSRC count!"
               BLANK_LINE);
        return 1;
    }

    return 0;
}
