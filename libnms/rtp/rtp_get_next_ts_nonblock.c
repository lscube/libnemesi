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
#include <nemesi/rtpptdefs.h>

double rtp_get_next_ts_nonblock(struct rtp_ssrc *stm_src)
{
	rtp_pkt *pkt;
//	double ts;
	
	pthread_mutex_lock(&(stm_src->po.po_mutex));
	
	if(stm_src->po.potail >= 0) {
		pthread_mutex_unlock(&(stm_src->po.po_mutex));
		pkt=(rtp_pkt *)(*(stm_src->po.bufferpool)+stm_src->po.potail);
		return /*ts=*/((double)(ntohl(pkt->time) - stm_src->ssrc_stats.firstts))/(double)rtp_pt_defs[pkt->pt].rate;
	
//		return ts;
	} else {
		pthread_mutex_unlock(&(stm_src->po.po_mutex));
		return -1;
	}
}
