/* * 
 *  $Id:rtcp_send_rr.c 267 2006-01-12 17:19:45Z shawill $
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

int rtcp_send_rr(rtp_session * rtp_sess)
{
	rtcp_pkt *pkt;
	int len;
	uint32 rr_buff[MAX_PKT_SIZE];
	rtp_ssrc *stm_src;

	memset(rr_buff, 0, MAX_PKT_SIZE * sizeof(uint32));
	pkt = (rtcp_pkt *) rr_buff;

	len = rtcp_build_rr(rtp_sess, pkt);	/* in 32 bit words */
	pkt = (rtcp_pkt *) (rr_buff + len);
	len += rtcp_build_sdes(rtp_sess, pkt, (MAX_PKT_SIZE >> 2) - len);	/* in 32 bit words */

	for (stm_src = rtp_sess->ssrc_queue; stm_src; stm_src = stm_src->next)
		if ( !(stm_src->no_rtcp) && stm_src->rtp_sess->rtcpfd > 0) {
			if (sendto(stm_src->rtp_sess->rtcpfd, rr_buff, (len << 2),
				   0, stm_src->rtcp_from.addr,
				   stm_src->rtcp_from.addr_len) < 0)
				nms_printf(NMSML_WARN,
					   "WARNING! Error while sending RTCP pkt\n");
			else
				nms_printf(NMSML_DBG1,
					   "RTCP RR packet sent\n");
		}

	return len;
}
