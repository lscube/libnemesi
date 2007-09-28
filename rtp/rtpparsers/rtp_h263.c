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
 * @file rtp_h263.c
 * H263 depacketizer RFC 4629
 */

static rtpparser_info h263_served = {
    -1,
    {"H263-1998", NULL}
};

static int h263_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;

    uint8_t *buf;
    size_t len; /* payload size, minus additional headers,
                 * plus the 2 zeroed bytes
                 */

    size_t start; /* how many bytes we are going to skip from the start */
    size_t offset = 0;

    void *priv = ssrc->privs[fr->pt];

    int err = RTP_FILL_OK;
    int marker;

    fr->len = 0;

    do {
        start = 2;
        if (!(pkt = rtp_get_pkt(ssrc, &len)))
            return RTP_BUFF_EMPTY;

        buf = RTP_PKT_DATA(pkt);
        len = RTP_PAYLOAD_SIZE(pkt, len);
        marker = RTP_PKT_MARK(pkt);

        if (buf[0]&0x4) { // p bit - we overwrite the first 2 bytes with zero
            start -= 2;
        }

        if (buf[0]&0x2) // v bit - skip one more
            ++start;

    //    fprintf(stderr, "P: %u V: %u PLEN: %u : %u + %u ------\n", header.p, header.v, PLEN, header.plen1, header.plen2);

        start += (buf[1]>>3)|((buf[0]&0x1)<<5); // plen - skip that many bytes

        len -= start;

        priv = fr->data = realloc(priv, len + offset);

        memcpy(fr->data + offset, buf + start, len);
        if (buf[0]&0x4)
            memset(fr->data + offset, 0, 2);

        fr->len = len + offset;

        offset += len;

        rtp_rm_pkt(ssrc);
    } while (!marker);

    memset(config, 0, sizeof(rtp_buff));

    return err;
}

RTP_PARSER(h263);

