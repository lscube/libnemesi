/*
 * Copyright (c) 2006 Ryan Martell. (rdm4@martellventures.com)
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

int nms_base64_decode(uint8_t * out, const char *in, int out_length);
int nms_hex_decode(uint8_t * out, const char *in, int out_length);
unsigned int nms_xiphlacing(unsigned char *s, unsigned int v);

inline uint64_t nms_consume_BE(uint8_t ** buff, uint8_t n_bytes);
inline uint32_t nms_consume_BE4(uint8_t ** buff);
inline uint32_t nms_consume_BE3(uint8_t ** buff);
inline uint16_t nms_consume_BE2(uint8_t ** buff);

#define nms_consume_1(buff) *((uint8_t*)(*(buff))++)
int nms_get_attr_value(char *attr, const char *param, char *v, int v_len );

int nms_alloc_data(uint8_t **buf, long *cur_len, long new_len);
void nms_append_data(uint8_t *dst, long offset, uint8_t *src, long len);
inline void nms_append_incr(uint8_t *dst, long *offset, uint8_t *src, long len);
