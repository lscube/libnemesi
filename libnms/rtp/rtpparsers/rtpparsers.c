/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
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

#include <nemesi/comm.h>
#include <nemesi/utils.h>

#include "rtpparsers.h"

extern rtpparser rtp_parser_mpa;
extern rtpparser rtp_parser_mpv;

rtpparser *rtpparsers[] = {
	&rtp_parser_mpa,
	&rtp_parser_mpv,
	NULL
};

static int rtp_def_parser(rtp_ssrc *, rtp_frame *fr);

static rtp_parser rtp_parsers[128] = {
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser,
	rtp_def_parser, rtp_def_parser, rtp_def_parser, rtp_def_parser
};

void rtp_parsers_init(void)
{
	int i;
	
	for (i=0; rtpparsers[i]; i++) {
		if (rtpparsers[i]->served->static_pt < 96) {
			rtp_parsers[rtpparsers[i]->served->static_pt] = rtpparsers[i]->rtp_parse;
			nms_printf(NMSML_DBG1, "Added rtp parser for pt %d\n", rtpparsers[i]->served->static_pt);
		} else
			nms_printf(NMSML_ERR, "rtp framer could not serve %d (>=96) payload as static... rejected\n");
	}
}

int rtp_parser_reg(int16 pt, char *mime)
{
	int i, j;
	
	if (pt < 96) {
		nms_printf(NMSML_ERR, "cannot dinamically register an rtp parser for static payload type (%d<96)\n");
		return RTP_REG_STATIC;
	}
	
	for (i=0; rtpparsers[i]; i++) {
		for (j=0; rtpparsers[i]->served->mime[j]; j++) {
			if ( !strcmpcase(rtpparsers[i]->served->mime[j], mime) ) {
				rtp_parsers[pt] = rtpparsers[i]->rtp_parse;
				return 0;
			}
		}
	}
	
	return 0;
}

rtp_parser *rtp_parsers_new(void)
{
	rtp_parser *new_defs;
	
	if ( !(new_defs=malloc(sizeof(rtp_parsers))) )
		return NULL;
		
	memcpy(new_defs, rtp_parsers, sizeof(rtp_parsers));
	
	return new_defs;
}

static int rtp_def_parser(rtp_ssrc *stm_src, rtp_frame *fr)
{
	// XXX tmp vars to be removed
	char dst[65535];
	size_t dst_size = sizeof(dst);
	// end of tmp vars
	rtp_pkt *pkt;
	size_t pkt_len, dst_used=0;
	size_t to_cpy;
	
	if ( !(pkt=rtp_get_pkt(stm_src, &pkt_len)) )
		return RTP_BUFF_EMPTY; // valid only for NON blocking version.
	
	fr->timestamp = RTP_PKT_TS(pkt);
	
	do {
		to_cpy = min(pkt_len, dst_size);
		memcpy(dst, RTP_PKT_DATA(pkt), to_cpy);
		dst_used += to_cpy;
		rtp_rm_pkt(stm_src);
	} while ( (dst_used<dst_size) && (pkt=rtp_get_pkt(stm_src, &pkt_len)) && (RTP_PKT_TS(pkt)==fr->timestamp) );
	
	return dst_used;
}
