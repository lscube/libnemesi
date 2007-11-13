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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rtpparser.h"

static rtpparser_info mpv_served = {
    32,
    {"MPV", NULL}
};

typedef struct {
    uint8_t *data;  //!< constructed frame, fragments will be copied there
    long len;       //!< buf length, it's the sum of the fragments length
    long data_size; //!< allocated bytes for data
    long timestamp; //!< timestamp of progressive frame
} rtp_mpv;

typedef struct {
#ifdef WORDS_BIGENDIAN
    uint32_t mbz:5;
    uint32_t t:1;
    uint32_t tr:10;
    uint32_t an:1;
    uint32_t n:1;
    uint32_t s:1;
    uint32_t b:1;
    uint32_t e:1;
    uint32_t p:3;
    uint32_t fbv:1;
    uint32_t bfc:3;
    uint32_t ffv:1;
    uint32_t ffc:3;
#else
    uint32_t tr_h:2;
    uint32_t t:1;
    uint32_t mbz:5;
    uint32_t tr_l:8;
    uint32_t p:3;
    uint32_t e:1;
    uint32_t b:1;
    uint32_t s:1;
    uint32_t n:1;
    uint32_t an:1;
    uint32_t ffc:3;
    uint32_t ffv:1;
    uint32_t bfc:3;
    uint32_t fbv:1;
#endif
    union {
        uint8_t data[1];
        struct {
            uint32_t ext_hdr;
            uint8_t data[1];
        } mpeg2;
    } pt;
} rtp_mpv_pkt;

#define RTP_MPV_PKT(pkt)        ((rtp_mpv_pkt *)(RTP_PKT_DATA(pkt)))
#define RTP_MPV_DATA(pkt)        (RTP_MPV_PKT(pkt)->t ? RTP_MPV_PKT(pkt)->pt.mpeg2.data : RTP_MPV_PKT(pkt)->pt.data)
#define RTP_MPV_DATA_LEN(pkt, pkt_size)    (RTP_MPV_PKT(pkt)->t ? RTP_PAYLOAD_SIZE(pkt, pkt_size)-8 : RTP_PAYLOAD_SIZE(pkt, pkt_size)-4)

#ifdef WORDS_BIGENDIAN
#define RTP_MPV_TR(pkt)            (RTP_MPV_PKT(pkt)->tr)
#else
#define RTP_MPV_TR(pkt)            (RTP_MPV_PKT(pkt)->tr_h << 8 | RTP_MPV_PKT(pkt)->tr_l)
#endif

static int mpv_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_mpv *priv = calloc(1, sizeof(rtp_mpv));

    if (!priv)
        return RTP_ERRALLOC;

    rtp_sess->ptdefs[pt]->priv = priv;

    return 0;
}

static int mpv_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{
    rtp_mpv *priv = ssrc->rtp_sess->ptdefs[pt]->priv;

    if (priv && priv->data)
        free(priv->data);
    if (priv)
        free(priv);

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    return 0;
}

static int mpv_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_mpv *priv = ssrc->rtp_sess->ptdefs[fr->pt]->priv;
    rtp_pkt *pkt;
    size_t pkt_len;
    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &pkt_len)))
        return RTP_BUFF_EMPTY;

    nms_printf(NMSML_DBG3, "\n[MPV]: header: mbz:%u t:%u tr:%u an:%u n:%u s:%u b:%u e:%u p:%u fbv:%u bfc:%u ffv:%u ffc:%u\n", RTP_MPV_PKT(pkt)->mbz, RTP_MPV_PKT(pkt)->t,
           RTP_MPV_TR(pkt),
           RTP_MPV_PKT(pkt)->an, RTP_MPV_PKT(pkt)->n,
           RTP_MPV_PKT(pkt)->s, RTP_MPV_PKT(pkt)->b,
           RTP_MPV_PKT(pkt)->e, RTP_MPV_PKT(pkt)->p,
           RTP_MPV_PKT(pkt)->fbv, RTP_MPV_PKT(pkt)->bfc,
           RTP_MPV_PKT(pkt)->ffv, RTP_MPV_PKT(pkt)->ffc);

    if (priv->len && (RTP_PKT_TS(pkt) != priv->timestamp)) {
        //incomplete packet without final fragment
        priv->len = 0;
        return RTP_PKT_UNKNOWN;
    }

    // discard pkt if it's fragmented and the first fragment was lost
    if (!priv->len && !RTP_MPV_PKT(pkt)->b) {
        rtp_rm_pkt(ssrc);
        return RTP_PKT_UNKNOWN;
    }

    pkt_len = RTP_MPV_DATA_LEN(pkt, pkt_len);

    if (priv->data_size < pkt_len + priv->len) {
        if (!(priv->data = realloc(priv->data, pkt_len + priv->len))) {
            return RTP_ERRALLOC;
        }
        priv->data_size = pkt_len + priv->len;
    }

    memcpy(priv->data + priv->len, RTP_MPV_DATA(pkt), pkt_len);
    priv->len += pkt_len;

    if (!RTP_PKT_MARK(pkt)) {
        priv->timestamp = RTP_PKT_TS(pkt);
        err = EAGAIN;
    } else {
        fr->data = priv->data;
        fr->len  = priv->len;
        priv->len = 0;
    }

    nms_printf(NMSML_DBG3, "fr->len: %d\n", fr->len);

    rtp_rm_pkt(ssrc);
    return err;
}

RTP_PARSER_FULL(mpv);
