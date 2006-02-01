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


#include "rtpparser.h"

static rtpparser_info served = {
	32,
	{"MPV", NULL}
};

RTPPRSR(mpv);

static int rtp_parse(rtp_fnc_type prsr_type, struct rtp_session *rtp_sess, struct rtp_ssrc *stm_src, char *dst, size_t dst_size, uint32 *timestamp)
{
	uint16 pt;
	rtp_pkt *pkt;
	size_t pkt_len; //, dst_used=0;
	size_t to_cpy;
	int dst_used=0, tot_pkts=0;
	
	if ( !(pkt=rtp_get_pkt(prsr_type, stm_src, (int *)&pkt_len)) )
		return RTP_BUFF_EMPTY; // valid only for NON blocking version.
	
	pt = RTP_PKT_PT(pkt);
	*timestamp = RTP_PKT_TS(pkt);
	
	do {
		tot_pkts += pkt_len;
		to_cpy = min(pkt_len, dst_size);
		memcpy(dst, RTP_PKT_DATA(pkt)+4, to_cpy);
		dst_used += to_cpy;
		rtp_rm_pkt(rtp_sess, stm_src);
	} while ( ((size_t)dst_used<dst_size) && (pkt=rtp_get_pkt(rtp_n_blk, stm_src, (int *)&pkt_len)) \
				&& (RTP_PKT_TS(pkt)==*timestamp) && !RTP_PKT_MARK(pkt) && (RTP_PKT_PT(pkt)==pt) );
	
	if (tot_pkts > dst_used)
		return RTP_DST_TOO_SMALL;
	else
		return dst_used;
#if 0
	do {
		to_cpy = min(pkt_len, dst_size);
		memcpy(dst, RTP_PKT_DATA(pkt)+4, to_cpy);
		dst_used += to_cpy;
		rtp_rm_pkt(rtp_sess, stm_src);
	} while ( (dst_used<dst_size) && (pkt=rtp_get_pkt(rtp_n_blk, stm_src, (int *)&pkt_len)) && (RTP_PKT_TS(pkt)==*timestamp) );
	
	return dst_used;
#endif
}
