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
 * @file rtp_h264.c
 * H264 depacketizer RFC 3984
 */

/**
 * Local structure, contains data necessary to compose a h264 frame out
 * of rtp fragments and set the correct timings.
 */

typedef struct {
    uint8_t *data;     //!< constructed frame, fragments will be copied there
    long len;          //!< buf length, it's the sum of the fragments length
    long data_size;    //!< allocated bytes for data
    unsigned long timestamp;    //!< timestamp of progressive frame
    uint8_t *conf;
    long conf_len;
    int configured;
} rtp_h264;

static rtpparser_info h264_served = {
    -1,
    {"H264", NULL}
};

static int h264_init_parser(rtp_session * rtp_sess, unsigned pt)
{
    rtp_h264 *priv = calloc(1, sizeof(rtp_h264));
    rtp_pt_attrs *attrs = &rtp_sess->ptdefs[pt]->attrs;
    char *value;
    int i;
    int len;

    if (!priv) return RTP_ERRALLOC;

    for (i=0; i < attrs->size; i++) {

        if ((value = nms_get_attr_value(attrs->data[i], "profile-level-id=",
            &len))) {
            if (len==6){ /*hex string*/}
        }
        if ((value = nms_get_attr_value(attrs->data[i], "packetization-mode",
            &len))) {
            // We do not support anything else.
            if (len != 1 && atoi(value) < 2) return RTP_PARSE_ERROR;
        }
        if ((value = strstr(attrs->data[i], "sprop-parameter-sets="))) {
        //shamelessly ripped from ffmpeg
            uint32_t start_seq = ntohl(1);
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
                    uint8_t *dest = calloc(1, packet_size +
                                           sizeof(start_seq) +
                                           priv->conf_len);
                    if(dest) {
                        if(priv->conf_len) {
                            memcpy(dest, priv->conf, priv->conf_len);
                            free(priv->conf);
                        }

                        memcpy(dest+priv->conf_len, &start_seq,
                               sizeof(start_seq));
                        memcpy(dest + priv->conf_len +  sizeof(start_seq),
                               decoded_packet, packet_size);

                        priv->conf = dest;
                        priv->conf_len += sizeof(start_seq) + packet_size;
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
    rtp_h264 *priv = ssrc->rtp_sess->ptdefs[pt]->priv;

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
    uint8_t start_seq[4] = {0, 0, 0, 1};
    int err = RTP_FILL_OK;

    if (!(pkt = rtp_get_pkt(ssrc, &len)))
        return RTP_BUFF_EMPTY;

    buf = RTP_PKT_DATA(pkt);
    len = RTP_PAYLOAD_SIZE(pkt, len);
    type = (buf[0] & 0x1f);

    // In order to produce a compliant bitstream, a PPS NALU should prefix
    // the data stream.
    if (!priv->configured && priv->conf_len) {
        if(nms_alloc_data(&priv->data, &priv->data_size, priv->conf_len)) {
            return RTP_ERRALLOC;
        }
        nms_append_incr(priv->data, &priv->len, priv->conf, priv->conf_len);
        priv->configured = 1;
    }

    if (priv->conf_len) {
        config->data = priv->conf;
        config->len = priv->conf_len;
    }

    if (type >= 1 && type <= 23) type = 1; // single packet

    switch (type) {
    case 0: // undefined;
        err = RTP_PKT_UNKNOWN;
        break;
    case 1:
        if(nms_alloc_data(&priv->data, &priv->data_size,
                          len + sizeof(start_seq) + priv->len)) {
            return RTP_ERRALLOC;
        }
        nms_append_incr(priv->data, &priv->len, start_seq, sizeof(start_seq));
        nms_append_incr(priv->data, &priv->len, buf, len);
        fr->data = priv->data;
        fr->len = priv->len;
        priv->len = 0;
        break;
    case 24:    // STAP-A (aggregate, output as whole or split it?)
        nms_printf (NMSML_WARN,"STAP-A - len: %u \n", len);
        #if 0
        {
            size_t frame_len;
            buf += priv->index + 1; // skip the nal
            frame_len = nms_consume_BE2(&buf);
            if (!frame_len) {
                nms_printf (NMSML_WARN,"Empty frame\n");
                err = RTP_PARSE_ERROR;
                break;
            }

            nms_printf(NMSML_WARN, "NAL: %d", *buf & 0x1f);

            if (frame_len + priv->index < len) {
                nms_printf (NMSML_WARN,"Packet size %d\n",frame_len);

                priv->data = fr->data = realloc(priv->data,
                                                sizeof(start_seq)
                                                + frame_len);
                memcpy(fr->data, &start_seq, sizeof(start_seq));
                memcpy(fr->data + sizeof(start_seq), buf, frame_len);
                fr->len = sizeof(start_seq) + frame_len;
                priv->index += 2 + frame_len;
                if ( priv->index + 1 < len) return err; // more to output
                else priv->index = 0; // get a new packet
            } else {
                nms_printf (NMSML_ERR,"STAP-A corrupted\n");
                err = RTP_PARSE_ERROR;
            }
        }
        break;
        #else
        // trim stap-a nalu header
        buf++;
        len--;
        // ffmpeg way
        {
            int pass = 0;
            int total_length = 0;

            for(pass= 0; pass<2; pass++) {
                uint8_t *src = buf;
                int src_len = len;
                do {
                    uint16_t nal_size = nms_consume_BE2(&src);
                    src_len -= 2;

                    if (nal_size <= src_len) {
                        if(pass==0) {
                            total_length += sizeof(start_seq) + nal_size;
                        } else {
                            nms_append_incr(priv->data, &priv->len, start_seq,
                                            sizeof(start_seq));
                            nms_append_incr(priv->data, &priv->len, src,
                                            nal_size);
                            nms_printf(NMSML_WARN,
                                       "NAL: %d - size %d \n",
                                       *src & 0x1f, nal_size);
                        }
                    } else {
                        nms_printf(NMSML_ERR,
                                   "nal size exceeds length: %d %d\n",
                                   nal_size, src_len);
                    }

                    src += nal_size;
                    src_len -= nal_size;

                    if (src_len < 0)
                        nms_printf(NMSML_ERR,
                                   "Consumed more bytes than we got! (%d)\n",
                                   src_len);
                } while (src_len > 2);  // because there could be rtp padding..

                if(pass==0) {
                    nms_printf (NMSML_WARN,"STAP-A - tot: %d \n", total_length);
                    if(nms_alloc_data(&priv->data, &priv->data_size,
                                      total_length + priv->len)) {
                        return RTP_ERRALLOC;
                    }
                }
            }
        }
        fr->data = priv->data;
        fr->len = priv->len;
        priv->len = 0;
        break;
        #endif
        /* Unsupported for now */
    case 25:    // STAP-B
        err = RTP_PKT_UNKNOWN;
        nms_printf (NMSML_WARN,"STAP-B\n");
        break;
    case 26:    // MTAP-16
        err = RTP_PKT_UNKNOWN;
        nms_printf (NMSML_WARN,"MTAP-16\n");
        break;
    case 27:    // MTAP-24
        nms_printf (NMSML_WARN,"MTAP-24\n");
        err = RTP_PKT_UNKNOWN;
        break;

    case 28:    // FU-A (fragmented nal, output frags or aggregate it)
        {
            uint8_t fu_indicator = nms_consume_1(&buf);  // read the fu_indicator
            uint8_t fu_header    = nms_consume_1(&buf);  // read the fu_header.
            uint8_t start_bit    = (fu_header & 0x80) >> 7;
            uint8_t end_bit      = (fu_header & 0x40) >> 6;
            uint8_t nal_type     = (fu_header & 0x1f);
            uint8_t reconstructed_nal;

            len -= 2; // skip fu indicator and header

            // reconstruct this packet's true nal; only the data follows..
            // the original nal forbidden bit and NRI are stored in
            // this packet's nal;
            reconstructed_nal = fu_indicator & 0xe0;
            reconstructed_nal |= nal_type;

            if(start_bit && !priv->len) {
                if(nms_alloc_data(&priv->data, &priv->data_size,
                                  len + 1 + sizeof(start_seq) + priv->len)) {
                    return RTP_ERRALLOC;
                }
                // copy in the start sequence, and the reconstructed nal....
                nms_append_incr(priv->data, &priv->len, start_seq,
                                sizeof(start_seq));
                nms_append_incr(priv->data, &priv->len, &reconstructed_nal, 1);
                nms_append_incr(priv->data, &priv->len, buf, len);
            } else {
                if (priv->timestamp != RTP_PKT_TS(pkt)) {
                    fr->len = priv->len = 0;
                    rtp_rm_pkt(ssrc);
                    return RTP_PKT_UNKNOWN;
                }
                if(nms_alloc_data(&priv->data, &priv->data_size,
                                  len + priv->len)) {
                    return RTP_ERRALLOC;
                }
                nms_append_incr(priv->data, &priv->len, buf, len);
            }

            priv->timestamp = RTP_PKT_TS(pkt);
            if (!end_bit) {
                err = EAGAIN;
            } else {
                fr->data = priv->data;
                fr->len = priv->len;
                priv->len = 0;
            }
        }
        break;
    case 30:                   // undefined
    case 31:                   // undefined
    default:
        err = RTP_PKT_UNKNOWN;
        break;
    }

    rtp_rm_pkt(ssrc);

    return err;
}

RTP_PARSER_FULL(h264);
