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


typedef struct
{
#if BYTE_ORDER == LITTLE_ENDIAN
    uint16_t plen1:1;       /* Length in bytes of the extra picture header */
    uint16_t v:1;           /* Video Redundancy Coding */
    uint16_t p:1;           /* picture/GOB/slice/EOS/EOSBS start code */
    uint16_t rr:5;          /* Reserved. Shall be zero. */

    uint16_t pebit:3;       /* Bits to ignore in the last byte of the header */
    uint16_t plen2:5;       /* Length in bytes of the extra picture header */
#elif BYTE_ORDER == BIG_ENDIAN
    uint16_t rr:5;          /* Reserved. Shall be zero. */
    uint16_t p:1;           /* picture/GOB/slice/EOS/EOSBS start code */
    uint16_t v:1;           /* Video Redundancy Coding */
    uint16_t plen1:1;       /* Length in bytes of the extra picture header */

    uint16_t plen2:5;       /* Length in bytes of the extra picture header */
    uint16_t pebit:3;       /* Bits to ignore in the last byte of the header */
#else
#error Neither big nor little
#endif
} h263_header;

static rtpparser_info h263_served = {
    -1,
    {"H263-1998", NULL}
};

static int h263_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    h263_header header;

    uint8_t *buf;
    size_t len;
    uint8_t PLEN = 0;

    size_t start = 0;
    size_t offset = 0;

    void *priv = ssrc->privs[fr->pt];

    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);

    memcpy(&header, buf, sizeof(h263_header));
    start += sizeof(h263_header);

    /* Splitted plen, blaaah! */
    PLEN |= header.plen1;
    PLEN = PLEN << 5;
    PLEN |= header.plen2;

    if (header.p)
        len += 2;

    if (!priv)
        priv = fr->data = calloc(1, len);
    else
        priv = fr->data = realloc(priv, len);

    if (header.p) {
        memset(fr->data, 0, 2);
        offset += 2;
    }

    if (header.v)
        ++start;

    fprintf(stderr, "P: %u V: %u PLEN: %u : %u + %u ------\n", header.p, header.v, PLEN, header.plen1, header.plen2);
    start += PLEN; 

    memcpy(fr->data + offset, buf + start, len);
    fr->len = len;

    rtp_rm_pkt(ssrc);

    return err;
}

RTP_PARSER(h263);

