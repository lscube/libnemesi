/* * 
 *  ./rtcp/parse_rtcp_sdes.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

#include <nemesi/rtcp.h>

int parse_rtcp_sdes(struct Stream_Source *stm_src, rtcp_pkt *pkt)
{
	int8 count=pkt->common.count;
	rtcp_sdes_t *sdes= &(pkt->r.sdes);
	rtcp_sdes_item_t *rsp, *rspn;
	rtcp_sdes_item_t *end=(rtcp_sdes_item_t *)((uint32 *)pkt + pkt->common.len + 1);
		
	while (--count >= 0){
		rsp=&(sdes->item[0]);
		if ( rsp >= end )
			break;
		for (; rsp->type; rsp=rspn){
			rspn = (rtcp_sdes_item_t *)((uint8 *)rsp + rsp->len + 2);
			if ( rspn >= end){
				rsp = rspn;
				break;
			}
			if(set_ssrc_sdes(stm_src, rsp))
				return 1;
		}
		sdes = (rtcp_sdes_t *)((uint32 *)sdes +(((uint8 *)rsp - (uint8 *)sdes) >> 2) + 1);
	}
	if (count >= 0)
		uiprintf("\nWARNING! Invalid RTCP SDES pkt format!\n");
	else
		stm_src->ssrc_stats.probation=0;
	return 0;
}
