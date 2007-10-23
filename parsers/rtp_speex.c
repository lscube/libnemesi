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

#include "rtpparser.h"
#include "rtp_utils.h"
#include <math.h>

/**
 * @file rtp_speex.c
 * SPEEX depacketizer
 */

static rtpparser_info speex_served = {
    -1,
    {"speex", NULL}
};

static int speex_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    uint8_t *buf;
    size_t len;
    void *priv = ssrc->privs[fr->pt];
    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);

    /* Check for PADDING BIT in RTP header and remove it.
     * The last octet of the padding is a count of how many padding octets
     * should be ignored, including itself (it will be a multiple of
     * four)
     */
    priv = fr->data = realloc(priv, len);
    memcpy(fr->data, buf, len);
    fr->len = len;

    rtp_rm_pkt(ssrc);

    memset(config, 0, sizeof(rtp_buff));

    return err;
}

RTP_PARSER(speex);
