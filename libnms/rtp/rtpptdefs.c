/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#define GLOBAL_RTP_DEFS
#include <nemesi/rtpptdefs.h>


/* pt  0 */ static rtp_audio pcmu = { "PCMU", AU, 8000, NULL, 1};
/* pt  1 */ static rtp_audio res_au = { "res", AU, 0, NULL, 0}; // pt 1,2,19
/* pt  2 = res_au (reserved audio) */
/* pt  3 */ static rtp_audio gsm = { "GSM", AU, 8000, NULL, 1};
/* pt  4 */ static rtp_audio g723 = { "G723", AU, 8000, NULL, 1};
/* pt  5 */ static rtp_audio dvi4_8000 = { "DVI4", AU, 8000, NULL, 1};
/* pt  6 */ static rtp_audio dvi4_16000 = { "DVI4", AU, 16000, NULL, 1};
/* pt  7 */ static rtp_audio lpc = { "LPC", AU, 8000, NULL, 1};
/* pt  8 */ static rtp_audio pcma = { "PCMA", AU, 8000, NULL, 1};
/* pt  9 */ static rtp_audio g722 = { "G722", AU, 8000, NULL, 1};
/* pt 10 */ static rtp_audio l16_2 = { "L16", AU, 44100, NULL, 2};
/* pt 11 */ static rtp_audio l16_1 = { "L16", AU, 44100, NULL, 1};
/* pt 12 */ static rtp_audio qcelp = { "QCELP", AU, 8000, NULL, 1};
/* pt 13 */ static rtp_audio cn = { "CN", AU, 8000, NULL, 1};
/* pt 14 */ static rtp_audio mpa = { "MPA", AU, 90000, NULL, 0};
/* pt 15 */ static rtp_audio g728 = { "G728", AU, 8000, NULL, 1};
/* pt 16 */ static rtp_audio dvi4_11025 = { "DVI4", AU, 11025, NULL, 1};
/* pt 17 */ static rtp_audio dvi4_22050 = { "DVI4", AU, 22050, NULL, 1};
/* pt 18 */ static rtp_audio g729 = { "G729", AU, 8000, NULL, 1};
/* pt 19 = res_au (reserved audio) */
/* pt 20-23 */ static rtp_audio unas_a = { "unas", AU, 0, NULL, 0}; // (unassigned audio)
/* pt 24 */ static rtp_video unas_v = { "unas", VI, 0, NULL}; // pt 24, 27, 29, 30
/* pt 25 */ static rtp_video celb = { "CelB", VI, 90000, NULL};
/* pt 26 */ static rtp_video jpeg = { "JPEG", VI, 90000, NULL};
/* pt 27 = unas_v (unassigned video) */
/* pt 28 */ static rtp_video nv = { "nv", VI, 90000, NULL};
/* pt 29 = unas_v (unassigned video) */
/* pt 30 = unas_v (unassigned video) */
/* pt 31 */ static rtp_video h261 = { "H261", VI, 90000, NULL};
/* pt 32 */ static rtp_video mpv = { "MPV", VI, 90000, NULL};
/* pt 33 */ static rtp_video mp2t = { "MP2T", VI, 90000, NULL};
/* pt 34 */ static rtp_video h263 = { "H263", VI, 90000, NULL};
/* pt 35-71 */ static rtp_pt unas = { "unas", NA, 0, NULL};
/* pt 72-76 */ static rtp_pt res = { "res", NA, 0, NULL};
/* pt 77-95 = unas (unassigned) */
/* pt 96-127 */ static rtp_pt dyn = { "dyn", NA, 0, NULL};

// other dynamically defined payload types
/* dyn */ static rtp_audio g726_40 = { "G726-40", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio g726_32 = { "G726-32", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio g726_24 = { "G726-24", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio g726_16 = { "G726-16", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio g729d = { "G729D", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio g729e = { "G729E", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio gsm_efr = { "GSM-EFR", AU, 8000, NULL, 1};
/* dyn */ static rtp_audio l8 = { "L8", AU, 0, NULL, 0};
/* dyn */ static rtp_audio red = { "RED", AU, 0, NULL, 0};
/* dyn */ static rtp_audio vdvi = { "VDVI", AU, 0, NULL, 1};
/* dyn */ static rtp_video h263_1998 = { "H@^#-1998", VI, 90000, NULL};

rtp_pt *rtp_pt_defs[128] = {
/*   0 -   3 */ RTP_PT(&pcmu),			RTP_PT(&res_au),		RTP_PT(&res_au),		RTP_PT(&gsm),
/*   4 -   7 */	RTP_PT(&g723),			RTP_PT(&dvi4_8000),		RTP_PT(&dvi4_16000),	RTP_PT(&lpc),
/*   8 -  11 */	RTP_PT(&pcma),			RTP_PT(&g722),			RTP_PT(&l16_2),			RTP_PT(&l16_1),
/*  12 -  15 */	RTP_PT(&qcelp),			RTP_PT(&cn),			RTP_PT(&mpa),			RTP_PT(&g728),
/*  16 -  19 */	RTP_PT(&dvi4_11025),	RTP_PT(&dvi4_22050),	RTP_PT(&g729),			RTP_PT(&res_au),
/*  20 -  23 */	RTP_PT(&unas_a),		RTP_PT(&unas_a),		RTP_PT(&unas_a),		RTP_PT(&unas_a),
/*  24 -  27 */	RTP_PT(&unas_v),		RTP_PT(&celb),			RTP_PT(&jpeg),			RTP_PT(&unas_v),
/*  28 -  31 */	RTP_PT(&nv),			RTP_PT(&unas_v),		RTP_PT(&unas_v),		RTP_PT(&h261),
/*  32 -  35 */	RTP_PT(&mpv),			RTP_PT(&mp2t),			RTP_PT(&h263),			&unas,
/*  36 -  39 */	&unas,					&unas,					&unas,					&unas,
/*  40 -  43 */	&unas,					&unas,					&unas,					&unas,
/*  44 -  47 */	&unas,					&unas,					&unas,					&unas,
/*  48 -  51 */	&unas,					&unas,					&unas,					&unas,
/*  52 -  55 */	&unas,					&unas,					&unas,					&unas,
/*  56 -  59 */	&unas,					&unas,					&unas,					&unas,
/*  60 -  63 */	&unas,					&unas,					&unas,					&unas,
/*  64 -  67 */	&unas,					&unas,					&unas,					&unas,
/*  68 -  71 */	&unas,					&unas,					&unas,					&unas,
/*  72 -  75 */	&res,					&res,					&res,					&res,
/*  76 -  79 */	&res,					&unas,					&unas,					&unas,
/*  80 -  83 */	&unas,					&unas,					&unas,					&unas,
/*  84 -  87 */	&unas,					&unas,					&unas,					&unas,
/*  88 -  91 */	&unas,					&unas,					&unas,					&unas,
/*  92 -  95 */	&unas,					&unas,					&unas,					&unas,
/*  96 -  99 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 100 - 103 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 104 - 107 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 108 - 111 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 112 - 115 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 116 - 119 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 120 - 123 */	&dyn,					&dyn,					&dyn,					&dyn,
/* 124 - 127 */	&dyn,					&dyn,					&dyn,					&dyn
};
