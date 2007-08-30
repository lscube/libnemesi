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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <nemesi/rtpptdefs.h>
#include <nemesi/utils.h>
#include <nemesi/comm.h>

// definitions of known payload types specified in RFC 3551

/* pt  0 */ static rtp_audio pcmu =
{ "PCMU", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };

#if 0
    /* we do not define reserved, unassigned or dynamic pt,
     * just initialize this position to NULL in rtpptdefs array */
                                                /* pt  1 */ static rtp_audio res_au =
{ "res", AU, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 0 };

                                                // pt 1,2,19
#endif
/* pt  2 = res_au (reserved audio) */
/* pt  3 */ static rtp_audio gsm =
{ "GSM", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt  4 */ static rtp_audio g723 =
{ "G723", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt  5 */ static rtp_audio dvi4_8000 =
{ "DVI4", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt  6 */ static rtp_audio dvi4_16000 =
{ "DVI4", AU, 16000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt  7 */ static rtp_audio lpc =
{ "LPC", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt  8 */ static rtp_audio pcma =
{ "PCMA", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt  9 */ static rtp_audio g722 =
{ "G722", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 10 */ static rtp_audio l16_2 =
{ "L16", AU, 44100, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 2 };
/* pt 11 */ static rtp_audio l16_1 =
{ "L16", AU, 44100, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 12 */ static rtp_audio qcelp =
{ "QCELP", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 13 */ static rtp_audio cn =
{ "CN", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 14 */ static rtp_audio mpa =
{ "MPA", AU, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 0 };
/* pt 15 */ static rtp_audio g728 =
{ "G728", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 16 */ static rtp_audio dvi4_11025 =
{ "DVI4", AU, 11025, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 17 */ static rtp_audio dvi4_22050 =
{ "DVI4", AU, 22050, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* pt 18 */ static rtp_audio g729 =
{ "G729", AU, 8000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };

/* pt 19 = res_au (reserved audio) */
#if 0
    /* we do not define reserved, unassigned or dynamic pt,
     * just initialize this position to NULL in rtpptdefs array */
                                                /* pt 20-23 */ static rtp_audio unas_a =
{ "unas", AU, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 0 };

                                                // (unassigned audio)
                                                /* pt 24 */ static rtp_video unas_v =
{ "unas", VI, 0, RTP_PT_ATTRS_INITIALIZER, NULL };

                                                // pt 24, 27, 29, 30
#endif
/* pt 25 */ static rtp_video celb =
{ "CelB", VI, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL };
/* pt 26 */ static rtp_video jpeg =
{ "JPEG", VI, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL };

/* pt 27 = unas_v (unassigned video) */
/* pt 28 */ static rtp_video nv =
{ "nv", VI, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL };

/* pt 29 = unas_v (unassigned video) */
/* pt 30 = unas_v (unassigned video) */
/* pt 31 */ static rtp_video h261 =
{ "H261", VI, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL };
/* pt 32 */ static rtp_video mpv =
{ "MPV", VI, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL };
/* pt 33 */ static rtp_audio_video mp2t =
{ "MP2T", AV, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 0 };
/* pt 34 */ static rtp_video h263 =
{ "H263", VI, 90000, 0, 0, RTP_PT_ATTRS_INITIALIZER, NULL };

#if 0
    /* we do not define reserved, unassigned or dynamic pt,
     * just initialize this position to NULL in rtpptdefs array */
/* pt 35-71 */ static rtp_pt unas =
{ "unas", NA, 0, RTP_PT_ATTRS_INITIALIZER, NULL };
/* pt 72-76 */ static rtp_pt res =
{ "res", NA, 0, RTP_PT_ATTRS_INITIALIZER, NULL };

/* pt 77-95 = unas (unassigned) */
/* pt 96-127 */ static rtp_pt dyn =
{ "dyn", NA, 0, RTP_PT_ATTRS_INITIALIZER, NULL };
#endif

#if 0                // we don't use them yet.
// other dynamically defined payload types
/* dyn */ static rtp_audio g726_40 =
{ "G726-40", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio g726_32 =
{ "G726-32", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio g726_24 =
{ "G726-24", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio g726_16 =
{ "G726-16", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio g729d =
{ "G729D", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio g729e =
{ "G729E", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio gsm_efr =
{ "GSM-EFR", AU, 8000, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_audio l8 =
{ "L8", AU, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 0 };
/* dyn */ static rtp_audio red =
{ "RED", AU, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 0 };
/* dyn */ static rtp_audio vdvi =
{ "VDVI", AU, 0, RTP_PT_ATTRS_INITIALIZER, NULL, 1 };
/* dyn */ static rtp_video h263_1998 =
{ "H263-1998", VI, 90000, RTP_PT_ATTRS_INITIALIZER, NULL };
#endif

static rtp_pt *rtp_pt_defs[128] = {
    /*   0 */ RTP_PT(&pcmu), /*   1 */ NULL /* res AU */ ,    /*   2 */
    NULL /* res AU */ , /*   3 */ RTP_PT(&gsm),
    /*   4 */ RTP_PT(&g723), /*   5 */ RTP_PT(&dvi4_8000),    /*   6 */
    RTP_PT(&dvi4_16000), /*   7 */ RTP_PT(&lpc),
    /*   8 */ RTP_PT(&pcma), /*   9 */ RTP_PT(&g722),    /*  10 */
    RTP_PT(&l16_2), /*  11 */ RTP_PT(&l16_1),
    /*  12 */ RTP_PT(&qcelp), /*  13 */ RTP_PT(&cn),    /*  14 */
    RTP_PT(&mpa),
    /*  15 */ RTP_PT(&g728),
    /*  16 */ RTP_PT(&dvi4_11025), /*  17 */ RTP_PT(&dvi4_22050), /*  18 */ RTP_PT(&g729),    /*  19 */
    NULL /* res AU */ ,
    /*  20 */ NULL /* unas AU */ , /*  21 */ NULL /* unas AU */ , /*  22 */ NULL /* unas AU */ ,    /*  23 */
    NULL /* unas AU */ ,
    /*  24 */ NULL /* unas VI */ , /*  25 */ RTP_PT(&celb),    /*  26 */
    RTP_PT(&jpeg), /*  27 */ NULL /* unas VI */ ,
    /*  28 */ RTP_PT(&nv), /*  29 */ NULL /* unas VI */ ,    /*  30 */
    NULL /* unas VI */ , /*  31 */ RTP_PT(&h261),
    /*  32 */ RTP_PT(&mpv), /*  33 */ RTP_PT(&mp2t),    /*  34 */
    RTP_PT(&h263), /*  35 */ NULL /* unas */ ,
    /*  36 */ NULL /* unas */ , /*  37 */ NULL /* unas */ ,    /*  38 */
    NULL /* unas */ , /*  39 */ NULL /* unas */ ,
    /*  40 */ NULL /* unas */ , /*  41 */ NULL /* unas */ ,    /*  42 */
    NULL /* unas */ , /*  43 */ NULL /* unas */ ,
    /*  44 */ NULL /* unas */ , /*  45 */ NULL /* unas */ ,    /*  46 */
    NULL /* unas */ , /*  47 */ NULL /* unas */ ,
    /*  48 */ NULL /* unas */ , /*  49 */ NULL /* unas */ ,    /*  50 */
    NULL /* unas */ , /*  51 */ NULL /* unas */ ,
    /*  52 */ NULL /* unas */ , /*  53 */ NULL /* unas */ ,    /*  54 */
    NULL /* unas */ , /*  55 */ NULL /* unas */ ,
    /*  56 */ NULL /* unas */ , /*  57 */ NULL /* unas */ ,    /*  58 */
    NULL /* unas */ , /*  59 */ NULL /* unas */ ,
    /*  60 */ NULL /* unas */ , /*  61 */ NULL /* unas */ ,    /*  62 */
    NULL /* unas */ , /*  63 */ NULL /* unas */ ,
    /*  64 */ NULL /* unas */ , /*  65 */ NULL /* unas */ ,    /*  66 */
    NULL /* unas */ , /*  67 */ NULL /* unas */ ,
    /*  68 */ NULL /* unas */ , /*  69 */ NULL /* unas */ ,    /*  70 */
    NULL /* unas */ , /*  71 */ NULL /* unas */ ,
    /*  72 */ NULL /* res */ , /*  73 */ NULL /* res */ ,    /*  74 */
    NULL /* res */ , /*  75 */ NULL /* res */ ,
    /*  76 */ NULL /* res */ , /*  77 */ NULL /* unas */ ,    /*  78 */
    NULL /* unas */ , /*  79 */ NULL /* unas */ ,
    /*  80 */ NULL /* unas */ , /*  81 */ NULL /* unas */ ,    /*  82 */
    NULL /* unas */ , /*  83 */ NULL /* unas */ ,
    /*  84 */ NULL /* unas */ , /*  85 */ NULL /* unas */ ,    /*  86 */
    NULL /* unas */ , /*  87 */ NULL /* unas */ ,
    /*  88 */ NULL /* unas */ , /*  89 */ NULL /* unas */ ,    /*  90 */
    NULL /* unas */ , /*  91 */ NULL /* unas */ ,
    /*  92 */ NULL /* unas */ , /*  93 */ NULL /* unas */ ,    /*  94 */
    NULL /* unas */ , /*  95 */ NULL /* unas */ ,
    /*  96 */ NULL /* dyn */ , /*  97 */ NULL /* dyn */ ,    /*  98 */
    NULL /* dyn */ , /*  99 */ NULL /* dyn */ ,
    /* 100 */ NULL /* dyn */ , /* 101 */ NULL /* dyn */ ,    /* 102 */
    NULL /* dyn */ , /* 103 */ NULL /* dyn */ ,
    /* 104 */ NULL /* dyn */ , /* 105 */ NULL /* dyn */ ,    /* 106 */
    NULL /* dyn */ , /* 107 */ NULL /* dyn */ ,
    /* 108 */ NULL /* dyn */ , /* 109 */ NULL /* dyn */ ,    /* 110 */
    NULL /* dyn */ , /* 111 */ NULL /* dyn */ ,
    /* 112 */ NULL /* dyn */ , /* 113 */ NULL /* dyn */ ,    /* 114 */
    NULL /* dyn */ , /* 115 */ NULL /* dyn */ ,
    /* 116 */ NULL /* dyn */ , /* 117 */ NULL /* dyn */ ,    /* 118 */
    NULL /* dyn */ , /* 119 */ NULL /* dyn */ ,
    /* 120 */ NULL /* dyn */ , /* 121 */ NULL /* dyn */ ,    /* 122 */
    NULL /* dyn */ , /* 123 */ NULL /* dyn */ ,
    /* 124 */ NULL /* dyn */ , /* 125 */ NULL /* dyn */ ,    /* 126 */
    NULL /* dyn */ , /* 127 */ NULL /* dyn */ ,
};

void rtpptdefs_new(rtp_pt * new_defs[])
{
    memcpy(new_defs, rtp_pt_defs, sizeof(rtp_pt_defs));
}

rtp_pt *rtp_pt_new(rtp_media_type mtype)
{
    rtp_pt *new;
    size_t struct_size;

    switch (mtype) {
    case AU:
        struct_size = sizeof(rtp_audio);
        break;
    case VI:
        struct_size = sizeof(rtp_video);
        break;
    case AV:
        struct_size = sizeof(rtp_audio_video);
        break;
    case NA:
        struct_size = sizeof(rtp_pt);
        break;
    default:
        return NULL;
        break;
    }

    if (!(new = calloc(1, struct_size)))
        return NULL;

    new->type = mtype;

    return new;
}

int rtp_dynpt_set(rtp_pt * defs[], rtp_pt * pt, uint8 value)
{
    if (value < 96) {
        nms_printf(NMSML_ERR,
               "You tried to set a non-dynamic payload type (%u)\n",
               value);
        return 1;
    }

    free(defs[value]);
    defs[value] = pt;

    return 0;
}

int rtp_dynpt_encname(rtp_pt * defs[], uint8 value, char *enc_name)
{
    if (value < 96) {
        nms_printf(NMSML_ERR,
               "You tried to set encoding name for non-dynamic payload type (%u)\n",
               value);
        return 1;
    }

    if (!defs[value]) {
        nms_printf(NMSML_ERR,
               "No dynamic payload type initialized for pt %u\n",
               value);
        return 1;
    }

    strncpy(defs[value]->name, enc_name, sizeof(defs[value]->name));
    defs[value]->name[sizeof(defs[value]->name)] = '\0';    // safety end-of-string

    return 0;
}

#if 0
// rtp_pt_attrs specific functions
void rtp_pt_attrs_init(rtp_pt_attrs * attrs)
{
    attrs->data = NULL;
    attrs->size = attrs->allocated = 0;
}
#endif

int rtp_pt_attr_add(rtp_pt * defs[], uint8 value, char *attr)
{
    rtp_pt_attrs *attrs;

    if (value > 127)
        return 1;

    attrs = &defs[value]->attrs;

    if (attrs->allocated == attrs->size) {    // we must realloc
        int minsize = min(attrs->allocated + 1, RTP_ATTRS_ARRAY_DEF_SIZE);
        if (! (attrs->data = realloc(attrs->data, minsize * sizeof(char*))))
            return -1;
        attrs->allocated = minsize;
    }
    if (!(attrs->data[attrs->size] = strdup(attr)))
        return -1;

    attrs->size++;

    return 0;
}
