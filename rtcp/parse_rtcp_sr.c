/* * 
 *  ./rtcp/parse_rtcp_sr.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:48 $
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

int parse_rtcp_sr(struct Stream_Source *stm_src, rtcp_pkt *pkt)
{
	gettimeofday(&(stm_src->ssrc_stats.lastsr), NULL);
	stm_src->ssrc_stats.ntplastsr[0]=ntohl(pkt->r.sr.si.ntp_seq);
	stm_src->ssrc_stats.ntplastsr[1]=ntohl(pkt->r.sr.si.ntp_frac);
	/* Per ora, non ci interessa altro. */
	/* Forse le altre informazioni possono */
	/* servire per un monitor RTP/RTCP */
	return 0;
}
