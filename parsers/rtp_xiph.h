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

/*
 * Common parsing for xiph format
 */

#define RTP_XIPH_ID(pkt)    (   (RTP_PKT_DATA(pkt)[0]<<16) +    \
                                (RTP_PKT_DATA(pkt)[1]<<8) +    \
                                (RTP_PKT_DATA(pkt)[2]))
#define RTP_XIPH_F(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0xc0)>> 6)
#define RTP_XIPH_T(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0x30)>> 4)
#define RTP_XIPH_PKTS(pkt)  (RTP_PKT_DATA(pkt)[3]& 0x0F)
#define RTP_XIPH_LEN(pkt,off)   ((RTP_PKT_DATA(pkt)[off])+    \
                                 (RTP_PKT_DATA(pkt)[off+1]<<8))
#define RTP_XIPH_DATA(pkt,off)  (RTP_PKT_DATA(pkt)+off+2)

/*
 * Generic configuration holder
 */

typedef struct {
    int id;     //!< Identification
    uint8_t *conf; //!< Configuration buffer
    long len;   //!< length
} rtp_xiph_conf;

/*
 * Bit I/O code from libogg
 */

/*
 * Copyright (c) 2002, Xiph.org Foundation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

typedef struct bit_context_s {
    long endbyte;
    int endbit;
    unsigned char *buffer;
    unsigned char *ptr;
    long storage;
} bit_context;

static inline long bit_read(bit_context * b, int bits)
{
  static const unsigned long mask[] =
    { 0x00000000, 0x00000001, 0x00000003, 0x00000007, 0x0000000f,
      0x0000001f, 0x0000003f, 0x0000007f, 0x000000ff, 0x000001ff,
      0x000003ff, 0x000007ff, 0x00000fff, 0x00001fff, 0x00003fff,
      0x00007fff, 0x0000ffff, 0x0001ffff, 0x0003ffff, 0x0007ffff,
      0x000fffff, 0x001fffff, 0x003fffff, 0x007fffff, 0x00ffffff,
      0x01ffffff, 0x03ffffff, 0x07ffffff, 0x0fffffff, 0x1fffffff,
      0x3fffffff, 0x7fffffff, 0xffffffff
    };

    long ret;
    unsigned long m = mask[bits];

    bits += b->endbit;

    if (b->endbyte + 4 >= b->storage) {
        /* not the main path */
        ret = -1L;
        if (b->endbyte * 8 + bits > b->storage * 8)
            goto overflow;
    }

    ret = b->ptr[0] >> b->endbit;
    if (bits > 8) {
        ret |= b->ptr[1] << (8 - b->endbit);
        if (bits > 16) {
            ret |= b->ptr[2] << (16 - b->endbit);
            if (bits > 24) {
                ret |= b->ptr[3] << (24 - b->endbit);
                if (bits > 32 && b->endbit) {
                    ret |= b->ptr[4] << (32 - b->endbit);
                }
            }
        }
    }
    ret &= m;

      overflow:

    b->ptr += bits / 8;
    b->endbyte += bits / 8;
    b->endbit = bits & 7;
    return (ret);
}

static inline void bit_readinit(bit_context * b, unsigned char *buf, int bytes)
{
    memset(b, 0, sizeof(*b));
    b->buffer = b->ptr = buf;
    b->storage = bytes;
}
