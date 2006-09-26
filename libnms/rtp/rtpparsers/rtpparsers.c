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

static int rtp_def_parser(rtp_ssrc *, rtp_frame * fr, rtp_buff * config);

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

static rtp_parser_init rtp_parsers_inits[128];

void rtp_parsers_init(void)
{
	int i;

	memset(rtp_parsers_inits, 0, sizeof(rtp_parsers_inits));

	for (i = 0; rtpparsers[i]; i++) {
		if (rtpparsers[i]->served->static_pt < 96) {
			rtp_parsers[rtpparsers[i]->served->static_pt] = rtpparsers[i]->parse;
			rtp_parsers_inits[rtpparsers[i]->served->static_pt] = rtpparsers[i]->init;
			nms_printf(NMSML_DBG1, "Added rtp parser for pt %d\n", rtpparsers[i]->served->static_pt);
		} else
			nms_printf(NMSML_ERR, "rtp framer could not serve %d (>=96) payload as static... rejected\n");
	}
}

int rtp_parser_reg(rtp_session * rtp_sess, int16 pt, char *mime)
{
	int i, j;

	if (pt < 96) {
		nms_printf(NMSML_ERR, "cannot dinamically register an rtp parser for static payload type (%d<96)\n");
		return RTP_REG_STATIC;
	}

	for (i = 0; rtpparsers[i]; i++) {
		for (j = 0; rtpparsers[i]->served->mime[j]; j++) {
			if (!strcmpcase(rtpparsers[i]->served->mime[j], mime)) {
				rtp_sess->parsers[pt] = rtpparsers[i]->parse;
				rtp_sess->parsers_inits[pt] = rtpparsers[i]->init;
				return RTP_OK;
			}
		}
	}

	return RTP_OK;
}

void rtp_parsers_new(rtp_parser * new_parsers, rtp_parser_init * new_parsers_inits)
{
	memcpy(new_parsers, rtp_parsers, sizeof(rtp_parsers));
	memcpy(new_parsers_inits, rtp_parsers_inits, sizeof(rtp_parsers_inits));
}

inline void rtp_parser_set_uninit(rtp_session * rtp_sess, unsigned pt, rtp_parser_uninit parser_uninit)
{
	rtp_sess->parsers_uninits[pt] = parser_uninit;
}

#define DEFAULT_PRSR_DATA_FRAME 65535

typedef struct {
	char *data;
	uint32 data_size;
} rtp_def_parser_s;

static int rtp_def_parser(rtp_ssrc * stm_src, rtp_frame * fr, rtp_buff * config)
{
	rtp_def_parser_s *priv = stm_src->privs[fr->pt];
	rtp_pkt *pkt;
	size_t pkt_len;
	uint32 tot_pkts = 0;

	if (!(pkt = rtp_get_pkt(stm_src, &pkt_len)))
		return RTP_BUFF_EMPTY;

	// fr->timestamp = RTP_PKT_TS(pkt);

	if (!priv) {
		nms_printf(NMSML_DBG3, "[rtp_def_parser] allocating new private struct...");
		if (!(stm_src->privs[fr->pt] = priv = malloc(sizeof(rtp_def_parser_s))))
			return RTP_ERRALLOC;
		priv->data_size = max(DEFAULT_PRSR_DATA_FRAME, pkt_len);
		if (!(fr->data = priv->data = malloc(priv->data_size)))
			return RTP_ERRALLOC;
		nms_printf(NMSML_DBG3, "done\n");
	} else
		fr->data = priv->data;

	do {
		pkt_len = RTP_PAYLOAD_SIZE(pkt, pkt_len);
		if (priv->data_size < tot_pkts + pkt_len) {
			nms_printf(NMSML_DBG3, "[rtp_def_parser] reallocating data...");
			if ((fr->data = priv->data = realloc(priv->data, tot_pkts + pkt_len)))
				return RTP_ERRALLOC;
			nms_printf(NMSML_DBG3, "done\n");
		}
		memcpy(fr->data + tot_pkts, RTP_PKT_DATA(pkt), pkt_len);
		tot_pkts += pkt_len;
		rtp_rm_pkt(stm_src);
	} while ((pkt = rtp_get_pkt(stm_src, &pkt_len)) && (RTP_PKT_TS(pkt) == fr->timestamp)
		 && (RTP_PKT_PT(pkt) == fr->pt));

	fr->len = tot_pkts;
	nms_printf(NMSML_DBG3, "fr->len: %d\n", fr->len);

	return RTP_FILL_OK;
}
