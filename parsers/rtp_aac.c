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
 * MPEG 4 Part 3 depacketizer RFC 3640
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
    uint8_t *conf;  //!< extradata
    long conf_len;  //!< extradata length 
    int size_len;   //!< Number of bits in the AU header for fragment size
    int index_len;  //!< Number of bits in the AU header for the index
    int delta_len;  //!< Number of bits in the AU header for the delta index
} rtp_aac;

static rtpparser_info aac_served = {
    -1,
    {"MPEG4-GENERIC", NULL}
};

static int aac_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_aac *priv = calloc(1, sizeof(rtp_aac));
    rtp_pt_attrs *attrs = &rtp_sess->ptdefs[pt]->attrs;
    char *value;
    uint8_t buffer[1024];
    int i, v_len, len, err = RTP_ERRALLOC;

    if (!priv)
        return RTP_ERRALLOC;

    for (i=0; i < attrs->size; i++){
        if ((value = nms_get_attr_value(attrs->data[i], "config", &v_len))) {
            if (!(v_len % 2) && v_len > 0) {
                if ((len = nms_hex_decode(buffer, value, sizeof(buffer))) < 0)
                    goto err_alloc;
                if (!(priv->conf = realloc(priv->conf, priv->conf_len + len)))
                    goto err_alloc;
                memcpy(priv->conf + priv->conf_len, buffer, len);
                priv->conf_len += len;
            }
        }
        if ((value = nms_get_attr_value(attrs->data[i], "mode", &v_len))) {
            if (strcmp(value,"AAC-hbr")) {
                nms_printf(NMSML_ERR, "Mode %s not supported\n",
                           value);
                err = RTP_PARSE_ERROR;
                goto err_alloc;
            }
        }
        if ((value = nms_get_attr_value(attrs->data[i], "sizeLength",
                &v_len))) {
            if ((priv->size_len = strtol(value, NULL, 0)) != 13) {
                nms_printf(NMSML_ERR, "Size Length %d not supported\n",
                           priv->size_len);
                err = RTP_PARSE_ERROR;
                goto err_alloc;
            }
        }
        if ((value = nms_get_attr_value(attrs->data[i], "indexLength",
                &v_len))) {
            if ((priv->index_len = strtol(value, NULL, 0)) != 3) {
                nms_printf(NMSML_ERR, "Index Length %d not supported\n",
                           priv->index_len);
                err = RTP_PARSE_ERROR;
                goto err_alloc;
            }
        }
        if ((value = nms_get_attr_value(attrs->data[i], "indexDeltaLength",
                &v_len))) {
            if ((priv->delta_len = strtol(value, NULL, 0)) != 3) {
                nms_printf(NMSML_ERR, "Index Delta Length %d not supported\n",
                           priv->index_len);
                err = RTP_PARSE_ERROR;
                goto err_alloc;
            }
        }
    }

    rtp_sess->ptdefs[pt]->priv = priv;

    return 0;

    err_alloc:
    if (priv->data)
        free(priv->data);
    free(priv);
    return err;
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
    int header_len, frame_len, frame_index; //XXX 16bit max so far

    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);
    header_len = ((buf[0]<<8)+buf[1]+7)/8; // the value in bits, little endian

    if (header_len != 2) {
        nms_printf(NMSML_WARN, "AAC Header size (%d) not supported yet\n", header_len);
        //rtp_rm_pkt(ssrc);
        //return RTP_PARSE_ERROR;
    }

    frame_len = (buf[2] << 5) + (buf[1]>>3); //XXX get size_len bits
    frame_index = buf[3] & 0x03;             //XXX get index_len bits


    if (priv->len && (RTP_PKT_TS(pkt) != priv->timestamp)) {
        //incomplete packet without final fragment
        priv->len = 0;
        return RTP_PKT_UNKNOWN;
    }

    if (priv->data_size < len + priv->len) {
        if (!(priv->data = realloc(priv->data, len + priv->len))) {
            return RTP_ERRALLOC;
        }
        priv->data_size = len + priv->len;
    }

    memcpy(priv->data + priv->len, buf + (header_len + 2), len - (header_len + 2));

    priv->len += len - (header_len + 2);

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
