/* * 
 *  ./include/nemesi/rtpptdefs.h: $Revision: 1.6 $ -- $Date: 2003/07/08 15:26:40 $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
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

#ifndef __RTP_PT_DEFS_H
#define __RTP_PT_DEFS_H

#include <config.h>

#include <nemesi/types.h>

#ifndef GLOBAL_RTP_DEFS
#define RTP_DEFS_EXTERN extern
#else /* GLOBAL_RTP_DEFS */
#define RTP_DEFS_EXTERN
#endif /* GLOBAL_RTP_DEFS */

#define RTP_DEF_CLK_RATE 8000

#define RTP_DEF_MAX_NAME_LEN 16

typedef enum {
	AU = 0,
	VI = 1,
	AV = 2,
	NA = 3
} rtp_media_type_t;

struct rtp_pt_def {
	char name[RTP_DEF_MAX_NAME_LEN];	/* Encoding Name */
	rtp_media_type_t type;			/* Media Type: (A)udio, (V)ideo,  (A)udio(/)(V)ideo, (N)ot(/)(A)pplicable */
	uint32 rate; 				/* Clock Rate - in Hertz*/
	uint8 channels;				/* Channels */
};

/*
extern struct rtp_pt_def rtp_pt_defs[];
extern int (*decoders[])(char *, int, uint8 *(*)());
*/

RTP_DEFS_EXTERN struct rtp_pt_def rtp_pt_defs[128]
#ifdef GLOBAL_RTP_DEFS
= {
/*   0 -   3 */	{ "PCMU", AU, 8000, 1},		{ "res", AU, 0, 0},		{ "G726-32", AU, 8000, 1},	{ "GSM", AU, 8000, 1},
/*   4 -   7 */	{ "res", AU, 0, 0},		{ "DVI4", AU, 8000, 1},		{ "DVI4", AU, 16000, 1},	{ "LPC", AU, 8000, 1},
/*   8 -  11 */	{ "PCMA", AU, 8000, 1},		{ "G722", AU, 8000, 1},		{ "L16", AU, 44100, 2},		{ "L16", AU, 44100,1},
/*  12 -  15 */	{ "QCELP", AU, 8000, 1},	{ "res", AU, 0, 0},		{ "MPA", AU, 90000, 0},		{ "G728", AU, 8000, 1},
/*  16 -  19 */	{ "DVI4", AU, 11025, 1},	{ "DVI4", AU, 22050, 1},	{ "G729", AU, 8000, 1},		{ "res", AU, 0, 0},
/*  20 -  23 */	{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},		{ "unas", AU, 0, 0},
/*  24 -  27 */	{ "unas", VI, 0, 0},		{ "CelB", VI, 90000, 0},	{ "JPEG", VI, 90000, 0},	{ "unas", VI, 0, 0},
/*  28 -  31 */	{ "nv", VI, 90000, 0},		{ "unas", VI, 0, 0},		{ "unas", VI, 0, 0},		{ "H261", VI, 90000, 0},
/*  32 -  35 */	{ "MPV", VI, 90000, 0},		{ "res", VI, 0, 0},		{ "res", VI, 0, 0},		{ "unas", NA, 0, 0},
/*  36 -  39 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  40 -  43 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  44 -  47 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  48 -  51 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  52 -  55 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  56 -  59 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  60 -  63 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  64 -  67 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  68 -  71 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  72 -  75 */	{ "res", NA, 0, 0},		{ "res", NA, 0, 0},		{ "res", NA, 0, 0},		{ "res", NA, 0, 0},
/*  76 -  79 */	{ "res", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  80 -  83 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  84 -  87 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  88 -  91 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  92 -  95 */	{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},		{ "unas", NA, 0, 0},
/*  96 -  99 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 100 - 103 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 104 - 107 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 108 - 111 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 112 - 115 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 116 - 119 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 120 - 123 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},
/* 124 - 127 */	{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0},		{ "dyn", NA, 0, 0}
}
#endif /* GLOBAL_RTP_DEFS */
;

RTP_DEFS_EXTERN int (*decoders[128])(char *, int, uint8 *(*)());

#endif
