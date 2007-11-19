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
 * @file rtp_aac.c
 * MPEG 4 Part 2 depacketizer RFC 3016
 */

/**
 * Local structure, contains data necessary to compose a aac frame out
 * of rtp fragments.
 */

typedef struct {
    uint8_t *data;  //!< constructed frame, fragments will be copied there
    long len;       //!< buf length, it's the sum of the fragments length
    long data_size; //!< allocated bytes for data
    long timestamp; //!< timestamp of progressive frame
    uint8_t *conf;
    long conf_len;
    int configured;
} rtp_aac;

static rtpparser_info aac_served = {
    -1,
    {"MP4V-ES", NULL}
};

static int aac_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_aac *priv = calloc(1, sizeof(rtp_aac));
    rtp_pt_attrs *attrs = &rtp_sess->ptdefs[pt]->attrs;
    char *value;
    uint8_t buffer[1024];
    int i, v_len, len;

    if (!priv)
        return RTP_ERRALLOC;

    for (i=0; i < attrs->size; i++){
        if ((value = strstr(attrs->data[i], "config="))) {
            value+=7;
            if (!((v_len = (strstr(value,";") - value)) % 2) && v_len > 0){
                /*hex string*/
                *(value + v_len) = '\0';
                if ((len = nms_hex_decode(buffer, value, sizeof(buffer))) < 0)
                    goto err_alloc;
                if (!(priv->conf = realloc(priv->conf, priv->conf_len + len)))
                    goto err_alloc;
                memcpy(priv->conf + priv->conf_len, buffer, len);
                priv->conf_len += len;
            }
        }
    }

    rtp_sess->ptdefs[pt]->priv = priv;

    return 0;

    err_alloc:
    if (priv->data)
        free(priv->data);
    free(priv);
    return RTP_ERRALLOC;
}

static int aac_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{
    rtp_aac *priv = ssrc->rtp_sess->ptdefs[pt]->priv;

    if (priv && priv->data)
        free(priv->data);
    if (priv && priv->conf)
        free(priv->conf);
    if (priv)
        free(priv);

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    return 0;
}

/**
 * it should return a aac frame by fetching one or more than a single rtp packet
 */

static int aac_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    uint8_t *buf;
    rtp_aac *priv = ssrc->rtp_sess->ptdefs[fr->pt]->priv;
    size_t len;

    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);

    if (priv->len && (RTP_PKT_TS(pkt) != priv->timestamp)) {
        //incomplete packet without final fragment
        priv->len = 0;
        return RTP_PKT_UNKNOWN;
    }

    // For the codec to work correctly, it may need a 'VOL Header' to be
    // inserted at the front of the data stream.
/*    if (!priv->configured && !priv->len && priv->conf_len) {
        if (!(priv->data = realloc(priv->data, priv->conf_len))) {
            return RTP_ERRALLOC;
        }
        priv->data_size = len;
        memcpy(priv->data, priv->conf, priv->conf_len);
        priv->len = priv->conf_len;
        priv->configured = 1;
    } */

    if (priv->data_size < len + priv->len) {
        if (!(priv->data = realloc(priv->data, len + priv->len))) {
            return RTP_ERRALLOC;
        }
        priv->data_size = len + priv->len;
    }

    memcpy(priv->data + priv->len, buf, len);

    priv->len += len;

    if (!RTP_PKT_MARK(pkt)) {
        priv->timestamp = RTP_PKT_TS(pkt);
        err = EAGAIN;
    } else {
        fr->data = priv->data;
        fr->len  = priv->len;
        priv->len = 0;
    }

    if (priv->conf_len) {
        config->data = priv->conf;
        config->len = priv->conf_len;
    }

    rtp_rm_pkt(ssrc);
    return err;
}

RTP_PARSER_FULL(aac);
