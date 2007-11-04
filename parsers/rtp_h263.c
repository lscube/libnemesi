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

/**
 * Local structure, contains data necessary to compose a h263 frame out
 * of rtp fragments.
 */

typedef struct {
    char *data;     //!< constructed frame, fragments will be copied there
    long len;       //!< buf length, it's the sum of the fragments length
    long data_size; //!< allocated bytes for data
    long timestamp; //!< timestamp of progressive frame
} rtp_h263;

static rtpparser_info h263_served = {
    -1,
    {"H263-1998", NULL}
};

static int h263_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_h263 *priv = calloc(1, sizeof(rtp_h263));

    if (!priv)
        return RTP_ERRALLOC;

    rtp_sess->ptdefs[pt]->priv = priv;

    return 0;
}

static int h263_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{
    rtp_h263 *priv = ssrc->rtp_sess->ptdefs[pt]->priv;

    if (priv && priv->data)
        free(priv->data);
    if (priv)
        free(priv);

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    return 0;
}

/**
 * it should return a h263 frame by fetching one or more than a single rtp packet
 */

static int h263_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    uint8_t *buf;
    rtp_h263 *priv = ssrc->rtp_sess->ptdefs[fr->pt]->priv;
    size_t len; /* payload size, minus additional headers,
                 * plus the 2 zeroed bytes
                 */
    size_t start = 2; /* how many bytes we are going to skip from the start */
    int err = RTP_FILL_OK;
    int p_bit;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);

    if (priv->len && (RTP_PKT_TS(pkt) != priv->timestamp)) {
        //incomplete packet without final fragment
        priv->len = 0;
        return RTP_PKT_UNKNOWN;
    }

    p_bit = buf[0] & 0x4;

    if (p_bit) { // p bit - we overwrite the first 2 bytes with zero
        start = 0;
    }

    if (!priv->len && !p_bit) {
        //incomplete packet without initial fragment
        rtp_rm_pkt(ssrc);
        return RTP_PKT_UNKNOWN;
    }

    if (buf[0]&0x2) // v bit - skip one more
        ++start;

    start += (buf[1]>>3)|((buf[0]&0x1)<<5); // plen - skip that many bytes

    len -= start;

    if (priv->data_size < len + priv->len) {
        if (!(priv->data = realloc(priv->data, len + priv->len))) {
            return RTP_ERRALLOC;
        }
        priv->data_size = len + priv->len;
    }

    memcpy(priv->data + priv->len, buf + start, len);
    if (p_bit) // p bit - we overwrite the first 2 bytes with zero
        memset(priv->data + priv->len, 0, 2);

    priv->len += len;

    if (!RTP_PKT_MARK(pkt)) {
        priv->timestamp = RTP_PKT_TS(pkt);
        err = EAGAIN;
    } else {
        fr->data = priv->data;
        fr->len  = priv->len;
        priv->len = 0;
    }

    memset(config, 0, sizeof(rtp_buff));

    rtp_rm_pkt(ssrc);
    return err;
}

RTP_PARSER_FULL(h263);
