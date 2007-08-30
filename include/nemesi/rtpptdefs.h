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

#ifndef __RTP_PT_DEFS_H
#define __RTP_PT_DEFS_H
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <nemesi/types.h>

#define RTP_DEF_CLK_RATE 8000

#define RTP_DEF_MAX_NAME_LEN 16

typedef enum {
    AU = 0,
    VI = 1,
    AV = 2,
    NA = 3
} rtp_media_type;

#define RTP_ATTRS_ARRAY_DEF_SIZE 3

typedef struct {
    char **data;
    uint32 size;
    uint32 allocated;
} rtp_pt_attrs;

#define RTP_PT_ATTRS_INITIALIZER {NULL, 0, 0}

#define RTP_PT_COMMON_FIELDS \
        char name[RTP_DEF_MAX_NAME_LEN]; /*!< Encoding Name */             \
        rtp_media_type type; /*!< Media Type: (A)udio, (V)ideo,            \
                                        (A)udio(/)(V)ideo,                 \
                                        (N)ot(/)(A)pplicable */            \
        unsigned rate; /*!< Clock Rate - in Hertz*/                        \
        unsigned fps;   /*!< Current frame per second   */                 \
        long prev_timestamp; \
        rtp_pt_attrs attrs; /*!< fmtp attribute strings from sdp           \
                                 description */                            \
        void *priv;         /*!< private data for rtp payload type */

/* XXX audio and video fields must have different names, 
 * because they are used together in rtp_audio_video
 */
#define RTP_AUDIO_FIELDS    uint8 channels;    /* Channels */

#define RTP_VIDEO_FIELDS

typedef struct {
RTP_PT_COMMON_FIELDS RTP_AUDIO_FIELDS} rtp_audio;

typedef struct {
RTP_PT_COMMON_FIELDS RTP_VIDEO_FIELDS} rtp_video;

typedef struct {
RTP_PT_COMMON_FIELDS RTP_AUDIO_FIELDS RTP_VIDEO_FIELDS} rtp_audio_video;

typedef struct rtp_pt_def {
RTP_PT_COMMON_FIELDS} rtp_pt;

#define RTP_FMTS_ARRAY_DEF_SIZE 3
typedef struct _rtp_fmts_list {
    unsigned pt;
    rtp_pt *rtppt;
    struct _rtp_fmts_list *next;
} rtp_fmts_list;
#define RTP_FMTS_INITIALIZER {0, NULL, NULL}

#define RTP_PT(x) ((rtp_pt *)x)
#define RTP_AUDIO(x) ((rtp_audio *)x)
#define RTP_VIDEO(x) ((rtp_video *)x)
#define RTP_AUDIO_VIDEO(x) ((rtp_audio_video *)x)

//rtp_pt **rtpptdefs_new(void);
void rtpptdefs_new(rtp_pt *[]);
rtp_pt *rtp_pt_new(rtp_media_type mtype);
int rtp_dynpt_set(rtp_pt * defs[], rtp_pt * pt, uint8 value);
int rtp_dynpt_encname(rtp_pt * defs[], uint8 value, char *enc_name);

//!rtp_pt_attrs specific functions
//void rtp_pt_attrs_init(rtp_pt_attrs *);
int rtp_pt_attr_add(rtp_pt * defs[], uint8 value, char *);

#endif
