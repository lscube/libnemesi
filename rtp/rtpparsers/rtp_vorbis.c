/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *      
 *    Luca Barbato - lu_zero@gentoo.org
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include "rtpparser.h"
#include "rtp_xiph.h"
#include <math.h>

/**
 * @file rtp_vorbis.c
 * Vorbis depacketizer - draft 03
 */

/**
 * Local structure, contains data necessary to compose a vorbis frame out
 * of rtp fragments and set the correct timings.
 */

typedef struct vorbis_s {
    long offset;    //!< offset across an aggregate rtp packet
    int pkts;       //!< number of packets yet to process in the aggregate
    char *buf;      //!< constructed frame, fragments will be copied there 
    long len;       //!< buf length, it's the sum of the fragments length
    long timestamp; //!< calculated timestamp
    int id;         //!< Vorbis id, it could change across packets.
    int modes;                  //!< Internal vorbis data
    long blocksizes[2];         //!< Internal vorbis data
    int param_blockflag[64];    //!< Internal vorbis data
    long prev_bs;               //!< Internal vorbis data
    long curr_bs;               //!< Internal vorbis data
} rtp_vorbis;

static rtpparser_info served = {
    -1,
    {"audio/vorbis", NULL}
};

RTP_PARSER_FULL(vorbis);

//helpers
static int rtp_ilog(unsigned int v)
{
    int ret = 0;
    while (v) {
        ret++;
        v >>= 1;
    }
    return (ret);
}

static int icount(unsigned int v)
{
    int ret = 0;
    while (v) {
        ret += v & 1;
        v >>= 1;
    }
    return (ret);
}

static long maptype_quantvals(int entries, int dim)
{
    long vals = floor(pow((float) entries, 1.f / dim));

    /* the above *should* be reliable, but we'll not assume that FP is
       ever reliable when bitstream sync is at stake; verify via integer
       means that vals really is the greatest value of dim for which
       vals^b->bim <= b->entries */
    /* treat the above as an initial guess */
    while (1) {
        long acc = 1;
        long acc1 = 1;
        int i;
        for (i = 0; i < dim; i++) {
            acc *= vals;
            acc1 *= vals + 1;
        }
        if (acc <= entries && acc1 > entries) {
            return (vals);
        } else {
            if (acc > entries) {
                vals--;
            } else {
                vals++;
            }
        }
    }
}

static int cfg_parse(rtp_vorbis * vorb, rtp_frame * fr)    //FIXME checks missing!
{
    bit_context opb;
    int num, i, j, k = -1, channels;

    //id packet
    vorb->blocksizes[0] = 1 << (fr->data[28] & 0x0f);
    vorb->blocksizes[1] = 1 << (fr->data[28] >> 4);
    channels = fr->data[11];
    //cb packet
    bit_readinit(&opb, fr->data + 30, fr->len - 30);
    if (5 != bit_read(&opb, 8))
        return RTP_PARSE_ERROR;

    for (i = 6; i > 0; i--)
        bit_read(&opb, 8);

    num = bit_read(&opb, 8) + 1;

    for (; num > 0; num--) {
        int entries, quantvals = -1, maptype, q_quant, dim;
        bit_read(&opb, 24);    //magic
        dim = bit_read(&opb, 16);    //dimensions
        entries = bit_read(&opb, 24);    //entries

        switch (bit_read(&opb, 1)) {
        case 0:    //unordered
            if (bit_read(&opb, 1)) {
                for (i = 0; i < entries; i++)
                    if (bit_read(&opb, 1))
                        bit_read(&opb, 5);
            } else
                for (i = 0; i < entries; i++)
                    bit_read(&opb, 5);
            break;
        case 1:    //ordered
            bit_read(&opb, 5);    //len
            for (i = 0; i < entries;)
                i += bit_read(&opb, rtp_ilog(entries - i));
            break;
        }
        switch ((maptype = bit_read(&opb, 4))) {
        case 0:
            break;

        case 1:
        case 2:
            bit_read(&opb, 32);
            bit_read(&opb, 32);
            q_quant = bit_read(&opb, 4) + 1;
            bit_read(&opb, 1);

            switch (maptype) {
            case 1:
                quantvals = maptype_quantvals(entries, dim);
                break;

            case 2:
                quantvals = entries * dim;
                break;
            }

            for (i = 0; i < quantvals; i++)
                bit_read(&opb, q_quant);
            break;
        }

    }

    //times
    num = bit_read(&opb, 6) + 1;
    for (; num > 0; num--)
        bit_read(&opb, 16);

    //floors
    num = bit_read(&opb, 6) + 1;
    for (; num > 0; num--)
        switch (bit_read(&opb, 16)) {

        case 0:
            bit_read(&opb, 16);
            bit_read(&opb, 16);
            bit_read(&opb, 16);
            bit_read(&opb, 6);
            bit_read(&opb, 8);
            for (i = bit_read(&opb, 4) + 1; i > 0; i--)
                bit_read(&opb, 8);
            break;
        case 1:
            {
                int pclass[31], pdim[16], rangebits,
                    partitions = bit_read(&opb, 5);
                for (i = 0; i < partitions; i++) {
                    pclass[i] = bit_read(&opb, 4);
                    if (k < pclass[i])
                        k = pclass[i];
                }
                for (j = 0; j < k + 1; j++) {
                    int subs;
                    pdim[j] = bit_read(&opb, 3) + 1;    // dim
                    subs = bit_read(&opb, 2);
                    if (subs)
                        bit_read(&opb, 8);    // book
                    for (i = 0; i < (1 << subs); i++)
                        bit_read(&opb, 8);
                }

                bit_read(&opb, 2);
                rangebits = bit_read(&opb, 4);
                for (j = 0, k = 0; j < partitions; j++) {
                    bit_read(&opb,
                         rangebits * pdim[pclass[j]]);
                }
            }
        }

    //residues
    num = bit_read(&opb, 6) + 1;
    for (; num > 0; num--) {
        int partitions, acc = 0;
        bit_read(&opb, 16);    // unpack is always the same
        bit_read(&opb, 24);    // begin
        bit_read(&opb, 24);    // end
        bit_read(&opb, 24);    // grouping
        partitions = bit_read(&opb, 6) + 1;
        bit_read(&opb, 8);    // groupbook
        for (j = 0; j < partitions; j++) {
            int cascade = bit_read(&opb, 3);
            if (bit_read(&opb, 1))
                cascade |= (bit_read(&opb, 5) << 3);
            acc += icount(cascade);
        }
        for (j = 0; j < acc; j++)
            bit_read(&opb, 8);
    }
    //maps
    num = bit_read(&opb, 6) + 1;
    for (; num > 0; num--) {
        int submaps = 1;
        bit_read(&opb, 16);    // maps is just mapping0
        if (bit_read(&opb, 1))
            submaps = bit_read(&opb, 4) + 1;
        if (bit_read(&opb, 1))
            for (i = bit_read(&opb, 8) + 1; i > 0; i--) {
                bit_read(&opb, rtp_ilog(channels - 1));
                bit_read(&opb, rtp_ilog(channels - 1));
            }
        bit_read(&opb, 2);
        if (submaps > 1)
            for (i = 0; i < channels; i++)
                bit_read(&opb, 4);
        for (i = 0; i < submaps; i++) {
            bit_read(&opb, 8);
            bit_read(&opb, 8);
            bit_read(&opb, 8);
        }
    }

    //modes
    vorb->modes = bit_read(&opb, 6) + 1;
    for (i = 0; i < vorb->modes; i++) {
        vorb->param_blockflag[i] = bit_read(&opb, 1);    //blockflag
        bit_read(&opb, 16);
        bit_read(&opb, 16);
        bit_read(&opb, 8);
    }

    return 0;        //FIXME add some checks and return -1 on failure
}

//get the blocksize from a full data packet
static long pkt_blocksize(rtp_vorbis * vorb, rtp_frame * fr)
{
    int mode;
    bit_context bc;
    bit_readinit(&bc, fr->data, fr->len);
    bit_read(&bc, 1);

    mode = bit_read(&bc, rtp_ilog(vorb->modes));

    return vorb->blocksizes[vorb->param_blockflag[mode]];
}

/**
 * Create standard mkv/nut/ffmpeg extradata from a configuration packet
 * layouts:
 * RTP: [Ident (30 bytes)][Setup (variable)]
 * extradata: [(1byte)number of fields-1][len0][len1][Ident][Comment][Setup]
 */
static int cfg_fixup(rtp_vorbis * vorb, rtp_frame * fr, rtp_buff * config,
             int id)
{
    uint8_t comment[26] =
        /*quite minimal comment */
    { 3, 'v', 'o', 'r', 'b', 'i', 's',
        10, 0, 0, 0,
        'v', 'o', 'r', 'b', 'i', 's', '-', 'r', 't', 'p',
        0, 0, 0, 0,
        1
    };
    int err = cfg_parse(vorb, fr);

    config->len = fr->len + 26 + 3;

    if (err)
        return err;

    config->data = realloc(config->data, config->len);
    config->data[0] = 2;
    config->data[1] = 30; //Ident len
    config->data[2] = 26; //Comment len

    memcpy(config->data + 3, fr->data, 30);         // id packet
    memcpy(config->data + 3 + 30, comment, 26);     // comment packet
    memcpy(config->data + 3 + 30 + 26, fr->data + 30, fr->len - 30);
    vorb->id = id;        //XXX
    //cfg_cache_append() //XXX
    return 0;
}

static int single_parse(rtp_vorbis * vorb, rtp_pkt * pkt, rtp_frame * fr,
            rtp_buff * config, rtp_ssrc * ssrc)
{

    uint32 len = RTP_XIPH_LEN(pkt, vorb->offset);

    if (vorb->id != RTP_XIPH_ID(pkt) ||    //not the current id
        //  !cfg_cache_find(vorb,RTP_XIPH_ID(pkt)) || //XXX
        RTP_XIPH_T(pkt) != 1    //not a config packet
        )
        return RTP_PARSE_ERROR;

    if (len > fr->len) {
        fr->data = realloc(fr->data, len);
        fr->len = len;
    }

    memcpy(fr->data, RTP_XIPH_DATA(pkt, vorb->offset), fr->len);
    vorb->pkts--;
    if (vorb->pkts == 0)
        rtp_rm_pkt(ssrc);

    if (RTP_XIPH_T(pkt) == 1)
        return cfg_fixup(vorb, fr, config, RTP_XIPH_ID(pkt));
    else {
        vorb->curr_bs = pkt_blocksize(vorb, fr);
        if (vorb->prev_bs)
            fr->timestamp += (vorb->curr_bs + vorb->prev_bs) / 4;
        vorb->prev_bs = vorb->curr_bs;
    }

    return 0;
}

static int pack_parse(rtp_vorbis * vorb, rtp_pkt * pkt, rtp_frame * fr,
              rtp_buff * config, rtp_ssrc * ssrc)
{
    single_parse(vorb, pkt, fr, config, ssrc);
    vorb->offset += fr->len + 2;
    return 0;
}

static int frag_parse(rtp_vorbis * vorb, rtp_pkt * pkt, rtp_frame * fr,
              rtp_buff * config, rtp_ssrc * ssrc)
{
    int len, err = 0;

    switch (RTP_XIPH_T(pkt)) {
    case 1:
        vorb->len = 0;
    case 2:
        len = RTP_XIPH_LEN(pkt, 4);
        vorb->buf = realloc(vorb->buf, vorb->len + len);
        memcpy(vorb->buf + vorb->len, RTP_XIPH_DATA(pkt, 4), len);
        vorb->len += len;
        //FIXME return value
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
            err = cfg_fixup(vorb, fr, config, RTP_XIPH_ID(pkt));
        else {
            vorb->curr_bs = pkt_blocksize(vorb, fr);
            if (vorb->prev_bs)
                fr->timestamp +=
                    (vorb->curr_bs + vorb->prev_bs) / 4;
            vorb->prev_bs = vorb->curr_bs;
        }
        break;
    default:
        err = -1;
        break;
    }

    rtp_rm_pkt(ssrc);

    return err;
}



static int rtp_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_vorbis *vorb = malloc(sizeof(rtp_vorbis));

    if (!vorb)
        return RTP_ERRALLOC;

    memset(vorb, 0, sizeof(rtp_vorbis));

// parse the sdp to get the first configuration

// setup the config cache //LATER

// associate it to the right payload

    rtp_sess->ptdefs[pt]->priv = vorb;

    return 0;
}

int rtp_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{

    rtp_vorbis *vorb = ssrc->privs[pt];

    if (vorb && vorb->buf)
        free(vorb->buf);
    if (vorb)
        free(vorb);

    vorb = ssrc->rtp_sess->ptdefs[pt]->priv;

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    if (vorb)
        free(vorb);

    return 0;
}

/**
 * it should return a vorbis frame either by unpacking an aggregate
 * or by fetching more than a single rtp packet
 */

static int rtp_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    size_t len;

    rtp_vorbis *vorb = ssrc->privs[fr->pt];

    if (!vorb) {
        vorb = ssrc->privs[fr->pt] = malloc(sizeof(rtp_vorbis));
        vorb =
            memcpy(vorb, ssrc->rtp_sess->ptdefs[fr->pt]->priv,
               sizeof(rtp_vorbis));
    }
    // get the current packet
    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    // if I don't have previous work
    if (!vorb->pkts) {
        // get the number of packets stuffed in the rtp
        vorb->pkts = RTP_XIPH_PKTS(pkt);

        // some error checking
        if (vorb->pkts > 0 && (RTP_XIPH_F(pkt) || !RTP_XIPH_T(pkt)))
            return RTP_PARSE_ERROR;

        if (RTP_XIPH_F(pkt))
            return frag_parse(vorb, pkt, fr, config, ssrc);
        // single packet, easy case
        if (vorb->pkts == 1)
            return single_parse(vorb, pkt, fr, config, ssrc);
        vorb->offset = 4;
    }
    // keep parsing the current rtp packet
    return pack_parse(vorb, pkt, fr, config, ssrc);
}
