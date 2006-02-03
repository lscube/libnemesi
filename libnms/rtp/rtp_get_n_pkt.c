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

/*! \brief This function returns a pointer to Nth packet in the bufferpool for
 * given playout buffer.
 * WARNING: the pointer returned is the memory space of the slot inside buffer pool:
 * Once the packet is decoded it must be removed from rtp queue using \see rtp_rm_pkt.
 * WARNING: returned pointer looks at a memory space not locked by mutex. This because
 * we suppose that there is only one reader for each playout buffer.
 * We lock mutex only for potail var reading.
 * \param len this is a return parameter for lenght of pkt. NULL value is allowed:
 * in this case, we understand that you are not interested about this value.
 * shawill: this function put his dirty hands on bufferpool internals!!!
 * \return the pointer to next packet in buffer or NULL if playout buffer is empty.
 * */ 
rtp_pkt *rtp_get_n_pkt(rtp_fnc_type fnc_type, struct rtp_ssrc *stm_src, int *len, uint32 n)
{ // TODO complete;
	int i;
	
	pthread_mutex_lock(&(stm_src->po.po_mutex));
	i = stm_src->po.potail;
	while ( (i >= 0) && (n-- > 0) ) {
		i = stm_src->po.pobuff[i].next;
	}
	if(i < 0) {
		if (fnc_type == rtp_blk)
			pthread_cond_wait(&(stm_src->po.cond_empty), &(stm_src->po.po_mutex));
		else {
			pthread_mutex_unlock(&(stm_src->po.po_mutex));
			return NULL;
		}
	}
	pthread_mutex_unlock(&(stm_src->po.po_mutex));
	
	
	if (len)
		*len = (stm_src->po.pobuff[i]).pktlen;
//	pthread_mutex_unlock(&(stm_src->po.po_mutex)); moved up
	
	return (rtp_pkt *)(*(stm_src->po.bufferpool)+i);
}
