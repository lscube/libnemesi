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

#include "rtpparser.h"
#include "rtp_utils.h"
#include <math.h>

/**
 * @file rtp_h264.c
 * H264 depacketizer RFC 3984
 */

/**
 * Local structure, contains data necessary to compose a h264 frame out
 * of rtp fragments and set the correct timings.
 */

typedef struct {
    char *data;     //!< constructed frame, fragments will be copied there
    long len;       //!< buf length, it's the sum of the fragments length
    uint8_t *conf;
    long conf_len;
    long timestamp;
} rtp_h264;

static rtpparser_info h264_served = {
    -1,
    {"H264", NULL}
};

static int h264_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_h264 *priv = malloc(sizeof(rtp_h264));
    rtp_pt_attrs *attrs = &rtp_sess->ptdefs[pt]->attrs;
    char *value;
    int i;

    if (!priv) return RTP_ERRALLOC;

    memset(priv, 0, sizeof(rtp_h264));

    for (i=0; i < attrs->size; i++){
        if ((value = strstr(attrs->data[i], "profile-level-id="))) {
            value+=17;
            if ((strstr(value,";")-value)==6){ /*hex string*/}
        }
        if ((value = strstr(attrs->data[i], "sprop-parameter-sets="))) {
        //shamelessly ripped from ffmpeg
            uint8_t start_sequence[]= { 0, 0, 1 };
            priv->conf_len = 0;
            priv->conf = NULL;
            value+=21;
            while (*value) {
                char base64packet[1024];
                uint8_t decoded_packet[1024];
                unsigned packet_size;
                char *dst = base64packet;

                while (*value && *value != ','
                       && (dst - base64packet) < sizeof(base64packet) - 1) {
                    *dst++ = *value++;
                }
                *dst++ = '\0';

                if (*value == ',')
                    value++;

                packet_size = nms_base64_decode(decoded_packet,
                                                base64packet,
                                                sizeof(decoded_packet));
                if (packet_size) {
                    uint8_t *dest = malloc(packet_size +
                                           sizeof(start_sequence) +
                                           priv->conf_len);
                    if(dest) {
                        if(priv->conf_len) {
                            memcpy(dest, priv->conf, priv->conf_len);
                            free(priv->conf);
                        }

                        memcpy(dest+priv->conf_len, start_sequence,
                               sizeof(start_sequence));
                        memcpy(dest + priv->conf_len +  sizeof(start_sequence),
                               decoded_packet, packet_size);

                        priv->conf = dest;
                        priv->conf_len += sizeof(start_sequence) + packet_size;
                    } else {
                       goto err_alloc;
                    }
                }
            }
        }
    }

    rtp_sess->ptdefs[pt]->priv = priv;

    return 0;

    err_alloc:
    free(priv);
    return RTP_ERRALLOC;
}

static int h264_uninit_parser(rtp_ssrc * ssrc, unsigned pt)
{
    rtp_h264 *priv = ssrc->privs[pt];

    if (priv && priv->data)
        free(priv->data);
    if (priv)
        free(priv);

    priv = ssrc->rtp_sess->ptdefs[pt]->priv;

    ssrc->rtp_sess->ptdefs[pt]->priv = NULL;

    if (priv)
        free(priv);

    return 0;
}

static rtp_pkt * h264_next_pkt(rtp_ssrc * ssrc, size_t * len, uint8_t ** buf)
{
        rtp_pkt * pkt;

        rtp_rm_pkt(ssrc);
        pkt = rtp_get_pkt(ssrc, len);
        if (!pkt) {
            *len = 0;
            *buf = NULL;
        }
        else {
            *len = RTP_PAYLOAD_SIZE(pkt, *len);
            *buf = RTP_PKT_DATA(pkt);
        }

        return pkt;
}

static int h264_check_if_packet_continues(rtp_pkt * pkt, rtp_frame * fr)
{
    if (!pkt) return 0;

    if (fr->timestamp == RTP_PKT_TS(pkt))
        return 1;

    return 0;
}

/**
 * it should return a h264 frame either by unpacking an aggregate
 * or by fetching more than a single rtp packet
 */

static int h264_parse(rtp_ssrc * ssrc, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    size_t len;
    rtp_h264 *priv = ssrc->rtp_sess->ptdefs[fr->pt]->priv;
    uint8_t *buf;
    uint8_t type;
    uint8_t start_sequence[]= {0, 0, 1};
    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;
    buf = RTP_PKT_DATA(pkt);
    type = (buf[0] & 0x1f);
    len = RTP_PAYLOAD_SIZE(pkt, len);

    if (type >= 1 && type <= 23) type = 1; // single packet

    switch (type) {
    case 0: // undefined;
        err = RTP_PKT_UNKNOWN;
        break;
    case 1:
        priv->data = fr->data =
            realloc(priv->data, sizeof(start_sequence)+len);
        memcpy(fr->data, start_sequence, sizeof(start_sequence));
        memcpy(fr->data+sizeof(start_sequence), buf, len);
        fr->len = sizeof(start_sequence)+len;
        break;
    case 24:    // STAP-A (aggregate, output as whole or split it?)

        /* Unsupported for now */
    case 25:    // STAP-B
    case 26:    // MTAP-16
    case 27:    // MTAP-24
        err = RTP_PKT_UNKNOWN;
        break;

    case 28:    // FU-A (fragmented nal, output frags or aggregate it)
        {
            uint8_t fu_indicator = *buf++;  // read the fu_indicator
            uint8_t fu_header    = *buf++;  // read the fu_header.
            uint8_t start_bit    = (fu_header & 0x80) >> 7;
            uint8_t end_bit      = (fu_header & 0x40) >> 6;
            uint8_t nal_type     = (fu_header & 0x1f);
            uint8_t reconstructed_nal;

            len -= 2; // skip fu indicator and header

            // reconstruct this packet's true nal; only the data follows..
            // the original nal forbidden bit and NRI are stored in
            // this packet's nal;
            reconstructed_nal = fu_indicator & (0xe0);
            reconstructed_nal |= nal_type;

            if(start_bit) {
                // copy in the start sequence, and the reconstructed nal....
                priv->data =
                    realloc (priv->data, sizeof(start_sequence) + 1 + len);
                memcpy(priv->data, start_sequence, sizeof(start_sequence));
                priv->data[sizeof(start_sequence)]= reconstructed_nal;
                memcpy(priv->data + sizeof(start_sequence) + 1, buf, len);
                priv->len = sizeof(start_sequence) + 1 + len;
                priv->timestamp = RTP_PKT_TS(pkt);
            } else {
                if (priv->timestamp != RTP_PKT_TS(pkt)) return RTP_PKT_UNKNOWN;
                priv->data =
                    realloc (priv->data, priv->len + len);
                memcpy(priv->data + priv->len, buf, len);
                priv->len += len;
            }

            if (!end_bit) {
                err = EAGAIN;
            } else {
                fr->data = priv->data;
                fr->len  = priv->len;
            }
        }
        break;
    case 30:                   // undefined
    case 31:                   // undefined
    default:
        err = RTP_PKT_UNKNOWN;
        break;
    }

    if (priv->conf_len) {
        config->data = priv->conf;
        config->len = priv->conf_len;
    }

    rtp_rm_pkt(ssrc);

    return err;
}

RTP_PARSER_FULL(h264);

