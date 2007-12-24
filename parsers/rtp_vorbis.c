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
#include "rtp_xiph.h"
#include "rtp_utils.h"
#include <math.h>

/**
 * @file rtp_vorbis.c
 * Vorbis depacketizer - draft 08
 */

/**
 * Local structure, contains data necessary to compose a vorbis frame out
 * of rtp fragments and set the correct timings.
 */

typedef struct {
    long offset;    //!< offset across an aggregate rtp packet
    int pkts;       //!< number of packets yet to process in the aggregate
    uint8_t *buf;   //!< constructed frame, fragments will be copied there
    long len;       //!< buf length, it's the sum of the fragments length
    int id;         //!< Vorbis id, it could change across packets.
    rtp_xiph_conf *conf;        //!< configuration list
    int conf_len;
} rtp_vorbis;

static rtpparser_info vorbis_served = {
    -1,
    {"vorbis", NULL}
};

static int single_parse(rtp_vorbis * vorb, rtp_pkt * pkt, rtp_frame * fr,
            rtp_buff * config, rtp_ssrc * ssrc)
{
    uint8_t * this_pkt = RTP_PKT_DATA(pkt) + vorb->offset;
    unsigned len = nms_consume_BE2(&this_pkt);

    if (vorb->id != RTP_XIPH_ID(pkt) &&    //not the current id
        //  !cfg_cache_find(vorb,RTP_XIPH_ID(pkt)) || //XXX
        (RTP_XIPH_T(pkt) != 1)    //not a config packet
       ) {
        nms_printf(NMSML_ERR, "Id %0x unknown, expected %0x\n",
                   (unsigned)RTP_XIPH_ID(pkt), (unsigned)vorb->id);
        return RTP_PARSE_ERROR;
    }

    fr->data = vorb->buf = realloc(vorb->buf, len);
    fr->len = vorb->len = len;

    memcpy(fr->data, this_pkt, fr->len);
    vorb->pkts--;
    if (vorb->pkts == 0) {
        rtp_rm_pkt(ssrc);
    }

    if (RTP_XIPH_T(pkt) == 1)
        return -1; //cfg_fixup(vorb, fr, config, RTP_XIPH_ID(pkt));
    else {
        config->data = vorb->conf[0].conf;
        config->len  = vorb->conf[0].len;
    }

    return 0;
}

static int frag_parse(rtp_vorbis * vorb, rtp_pkt * pkt, rtp_frame * fr,
              rtp_buff * config, rtp_ssrc * ssrc)
{
    int len, err = EAGAIN;

    switch (RTP_XIPH_F(pkt)) {
    case 1:
        vorb->len = 0;
    case 2:
        len = RTP_XIPH_LEN(pkt, 4);
        vorb->buf = realloc(vorb->buf, vorb->len + len);
        memcpy(vorb->buf + vorb->len, RTP_XIPH_DATA(pkt, 4), len);
        vorb->len += len;
        break;
    case 3:
        len = RTP_XIPH_LEN(pkt, 4);
        vorb->buf = realloc(vorb->buf, vorb->len + len);
        memcpy(vorb->buf + vorb->len, RTP_XIPH_DATA(pkt, 4), len);
        vorb->len += len;

        if (vorb->len > fr->len) {
            fr->data = realloc(fr->data, vorb->len);
            fr->len = vorb->len;
        }
        memcpy(fr->data, vorb->buf, fr->len);

        if (RTP_XIPH_T(pkt) == 1)
            err = -1;//cfg_fixup(vorb, fr, config, RTP_XIPH_ID(pkt));
        else {
            config->data = vorb->conf[0].conf;
            config->len  = vorb->conf[0].len;
            err = 0;
        }
        break;
    default:
        err = -1;
        break;
    }

    rtp_rm_pkt(ssrc);

    return err;
}

static int pack_parse(rtp_vorbis * vorb, rtp_pkt * pkt, rtp_frame * fr,
              rtp_buff * config, rtp_ssrc * ssrc)
{
    single_parse(vorb, pkt, fr, config, ssrc);
    vorb->offset += fr->len + 2;
    return 0;
}

static uint64_t get_v(uint8_t **cur, int *len)
{
    uint64_t val = 0;
    uint8_t *cursor = *cur;
    int tmp = 128, i;
    for (i = 0; i<*len && tmp&128; i++)
    {
        tmp = *cursor++;
        val= (val<<7) + (tmp&127);
    }
    *cur = cursor;
    *len -= i;
    return val;
}

static int xiphrtp_to_mkv(rtp_vorbis *vorb, uint8_t **value, int *size)
{
    uint8_t *conf;
    rtp_xiph_conf *tmp;
    int i, count, offset = 1, val;
    int id  = nms_consume_BE3(value);
    int len = nms_consume_BE2(value);

    //XXX check len

    if (len) {
        // convert the format
        count = get_v(value, size);
        if (count != 2) {
            // corrupted packet?
            return RTP_PARSE_ERROR;
        }
        conf = malloc(len + len/255 + 64);
        for (i=0; i < count && size > 0 && offset < len; i++) {
            val = get_v(value, size);
            offset += nms_xiphlacing(conf + offset, val);
        }
        if (len > *size) {
            free(conf);
            return RTP_PARSE_ERROR;
        }
        conf[0] = count;
        memcpy(conf + offset, *value, len);
        *value += len;
        // append to the list
        vorb->conf_len++;
        vorb->conf = realloc(vorb->conf,
                               vorb->conf_len*sizeof(rtp_xiph_conf));
        tmp = vorb->conf + vorb->conf_len - 1;
        tmp->conf = conf;
        tmp->len = len + offset;
        tmp->id = id;
    }
    return 0;
}


static int unpack_config(rtp_vorbis *vorb, char *value, int len)
{
    uint8_t buff[len];
    uint8_t *cur = buff;
    int size = nms_base64_decode(buff, value, len);
    int count, i;
    if (size <= 0) return 1;

    count = nms_consume_BE4(&cur);
    size -= 4;

    for (i = 0; i < count && size > 0; i++) {
        if (xiphrtp_to_mkv(vorb, &cur, &size)) return 1;
    }

    return 0;
}

static void cleanup (rtp_vorbis *vorb)
{
    int i;
    if (vorb->buf)
        free(vorb->buf);
    if (vorb->conf) {
        for (i = 0; i < vorb->conf_len; i++)
            if (vorb->conf[i].conf)
                free(vorb->conf[i].conf);
        free(vorb->conf);
    }

    free(vorb);
}

static int vorbis_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{
    rtp_vorbis *vorb = ssrc->rtp_sess->ptdefs[pt]->priv;

    if (!vorb) return 0;

    cleanup(vorb);

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    return 0;
}

static int vorbis_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_vorbis *vorb = calloc(1, sizeof(rtp_vorbis));
    rtp_pt_attrs *attrs = &rtp_sess->ptdefs[pt]->attrs;
    char value[65536];
    int i, err = -1;
    int len;

    if (!vorb)
        return RTP_ERRALLOC;

    memset(vorb, 0, sizeof(rtp_vorbis));

// parse the sdp to get the first configuration
    for (i=0; i < attrs->size; i++) {
        if ((len = nms_get_attr_value(attrs->data[i], "configuration",
             value, sizeof(value)))) {
            err = unpack_config(vorb, value, len);
        }
        // other ways are disregarded for now.
        if (!err) break;
    }

    if (err) {
        cleanup(vorb);
        return RTP_PARSE_ERROR;
    }

    // We start with the first codebook set
    vorb->id = vorb->conf[0].id;

    rtp_sess->ptdefs[pt]->priv = vorb;

    return 0;
}

/**
 * it should return a vorbis frame either by unpacking an aggregate
 * or by fetching more than a single rtp packet
 */

static int vorbis_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    size_t len;

    rtp_vorbis *vorb = ssrc->rtp_sess->ptdefs[fr->pt]->priv;

    config->data = vorb->conf[0].conf;
    config->len  = vorb->conf[0].len;

    // get the current packet
    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;
    /*fprintf(stderr, "ID: %d, Type: %d, Off: %d\n", RTP_XIPH_ID(pkt), RTP_XIPH_T(pkt), vorb->offset);*/
    // if I don't have previous work
    if (!vorb->pkts) {

        // get the number of packets stuffed in the rtp
        vorb->pkts = RTP_XIPH_PKTS(pkt);
        /*fprintf(stderr, "Pkt: %d\n", vorb->pkts);*/
        // some error checking
        if (vorb->pkts > 0 && ( (RTP_XIPH_F(pkt)) || (RTP_XIPH_T(pkt) !=0) )) {
            /*fprintf(stderr, "ERRORE\n");*/
            return RTP_PARSE_ERROR;
        }

        if (RTP_XIPH_F(pkt))
            return frag_parse(vorb, pkt, fr, config, ssrc);

        vorb->offset = 4;

        // single packet, easy case
        if (vorb->pkts == 1) {
            /*fprintf(stderr, "SINGL\n");*/
            return single_parse(vorb, pkt, fr, config, ssrc);
        }
    }
    // keep parsing the current rtp packet
    /*fprintf(stderr, "PACK\n");*/
    return pack_parse(vorb, pkt, fr, config, ssrc);
}

RTP_PARSER_FULL(vorbis);
