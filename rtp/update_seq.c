/* * 
 *  ./rtp/update_seq.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

void update_seq(struct Stream_Source *stm_src, uint16 seq)
{
	struct SSRC_Stats *stats=&(stm_src->ssrc_stats);
	uint16 udelta = seq - stats->max_seq;

	if(stats->probation) {
		if (seq == stats->max_seq + 1){
			stats->probation--;
			stats->max_seq = seq;
			if (stats->probation == 0){
				init_seq(stm_src, seq);
				stats->received++;
				return;
			}
		} else {
			stats->probation = MIN_SEQUENTIAL - 1;
			stats->max_seq = seq;
		}
		return;
	} else if ( udelta < MAX_DROPOUT ){
		if ( seq < stats->max_seq ){
			/*
			 * Sequence number wrapped - count another 64k cycle.
			 */
			stats->cycles+=RTP_SEQ_MOD;
		}
		stats->max_seq = seq;
	} else if ( udelta <= RTP_SEQ_MOD - MAX_MISORDER ){
		/* the sequence number made a very large jump */
		if ( seq == stats->bad_seq ){
			init_seq(stm_src, seq);
		} else {
			stats->bad_seq = (seq + 1) & ( RTP_SEQ_MOD-1 );
			return;
		}
	}/* else {
			duplicate or reorder packet
	}*/
	stats->received++;
	return;
}
