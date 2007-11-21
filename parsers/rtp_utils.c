/*
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * @file rtp_utils.c
 * Repository of miscellaneus functions ripped from ffmpeg
 */

#include <stdint.h>
#include <string.h>

static uint8_t map2[] =
{
    0x3e, 0xff, 0xff, 0xff, 0x3f, 0x34, 0x35, 0x36,
    0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x01,
    0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
    0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10, 0x11,
    0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x1a, 0x1b,
    0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23,
    0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2a, 0x2b,
    0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33
};

/**
 * decodes base64
 * param order as strncpy()
 * Copyright (c) 2006 Ryan Martell. (rdm4@martellventures.com)
 */

int nms_base64_decode(uint8_t * out, const char *in, int out_length)
{
    int i, v;
    uint8_t *dst = out;

    v = 0;
    for (i = 0; in[i] && in[i] != '='; i++) {
        unsigned int index= in[i]-43;
        if (index>=(sizeof(map2)/sizeof(map2[0])) || map2[index] == 0xff)
            return -1;
        v = (v << 6) + map2[index];
        if (i & 3) {
            if (dst - out < out_length) {
                *dst++ = v >> (6 - 2 * (i & 3));
            }
        } 
    }

    return (dst - out);
}

/**
 * decodes hex strings
 * param order as strncpy()
 */

int nms_hex_decode(uint8_t * out, const char *in, int out_length)
{
    int i, c = 0;
    uint8_t *dst = out;

    for (i = 0; in[i]; i++) {
        if (in[i] >= '0' && in[i] <= '9')
            c += in[i] - '0';
        else if (in[i] >= 'a' && in[i] <= 'f')
            c += in[i] - 'a' + 10;
        else if (in[i] >= 'A' && in[i] <= 'F')
            c += in[i] - 'A' + 10;
        else
            return -1;

        if (i % 2 && (dst - out < out_length)) {
            *dst++ = c;
        }
        c = c << 4 & 0xFF;
    }

    return (dst - out);
}

/*
 * xiph/mkv variable length encoder
 * @param s destination
 * @param v value
 * @return the number of byte written
 */
unsigned int nms_xiphlacing(unsigned char *s, unsigned int v)
{
    unsigned int n = 0;

    while(v >= 0xff) {
        *s++ = 0xff;
        v -= 0xff;
        n++;
    }
    *s = v;
    n++;
    return n;
}

uint64_t nms_consume_BE(uint8_t ** buff, uint8_t n_bytes)
{
    uint64_t v = 0;
    uint8_t left = n_bytes;

    while (left)
        v |= *((uint8_t*)(*buff)++) << ((--left)*8);

    return v;    
}

uint32_t nms_consume_BE4(uint8_t ** buff)
{
    uint32_t v = 0;
    uint8_t * buff_p = *buff;
   
    v = (buff_p[3])|(buff_p[2] << 8)|(buff_p[1] << 16)|(buff_p[0] << 24);
    *buff += 4;

    return v;
}

uint32_t nms_consume_BE3(uint8_t ** buff)
{
    uint32_t v = 0;
    uint8_t * buff_p = *buff;
   
    v = (buff_p[2])|(buff_p[1] << 8)|(buff_p[0] << 16);
    *buff += 3;

    return v;
}


uint16_t nms_consume_BE2(uint8_t ** buff)
{
    uint16_t v = 0;
    uint8_t * buff_p = *buff;
   
    v = (buff_p[1])|(buff_p[0] << 8);
    *buff += 2;

    return v;
}

/**
 * Looks for the value of a parameter within the attribute string
 * returns the pointer to the value and its size
 */

char *nms_get_attr_value(char *attr, const char *param, int *v_len )
{
    char *value, *tmp;
    int trash;
    if (!v_len)
        v_len = &trash;
    if ((value = strstr(attr, param)) &&
        (*(value += strlen(param)) == '=')) {
            value++;
            if ((tmp = strstr(value,";"))) {
                *v_len = tmp - value;
                value[*v_len] = '\0';
            } else {
                *v_len = strlen(value);
            }
        return value;
    }
    return NULL;
}
