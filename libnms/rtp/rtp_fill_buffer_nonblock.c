/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
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

#include <nemesi/rtp.h>

int rtp_fill_buffer_nonblock(struct rtp_session *rtp_sess, struct rtp_ssrc *stm_src, char *dst, size_t dst_size, uint32 *timestamp)
{	
	rtp_pkt *pkt;
	size_t pkt_len, dst_used=0;
	size_t to_cpy;
	
	if ( !(pkt=rtp_get_pkt_nonblock(stm_src, (int *)&pkt_len)) )
		return RTP_FILL_EMPTY;
	*timestamp = RTP_PKT_TS(pkt);
	
	do {
		to_cpy = min(pkt_len, dst_size);
		memcpy(dst, RTP_PKT_DATA(pkt), to_cpy);
		dst_used += to_cpy;
		rtp_rm_pkt(rtp_sess, stm_src);
	} while ( (dst_used<dst_size) && (pkt=rtp_get_pkt_nonblock(stm_src, (int *)&pkt_len)) && (RTP_PKT_TS(pkt)==*timestamp) );
	
	return dst_used;
}
