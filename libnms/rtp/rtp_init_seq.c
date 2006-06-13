/* * 
 *  $Id:rtp_init_seq.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtp.h>

void rtp_init_seq(rtp_ssrc *stm_src, uint16 seq)
{
	struct rtp_ssrc_stats *stats=&(stm_src->ssrc_stats);

	stats->base_seq = seq-1; // FIXME: in rfc 3550 it's set to seq.
	stats->max_seq = seq;
	stats->bad_seq = RTP_SEQ_MOD + 1;
	stats->cycles = 0;
	stats->received = 0;
	stats->received_prior = 0;
	stats->expected_prior = 1;
	
	// our initializations
	// enqueue this SSRC in active SSRCs queue of RTP session.
	stm_src->next_active = stm_src->rtp_sess->active_ssrc_queue;
	stm_src->rtp_sess->active_ssrc_queue = stm_src;
	
	return;
}
