/* * 
 *  ./rtcp/rtcp_recv.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

int rtcp_recv(struct RTP_Session *rtp_sess)
{
	uint8 buffer[1024];
	struct Stream_Source *stm_src;
	struct sockaddr_in server;
	rtcp_pkt *pkt;
	int ret, n, server_len=sizeof(struct sockaddr);

	memset(buffer, 0, 1024);
	
	n=recvfrom(rtp_sess->rtcpfd, buffer, 1024, 0, (struct sockaddr *)&server, &server_len);

	pkt=(rtcp_pkt *)buffer;

	if (rtcp_hdr_val_chk(pkt, n)){
		uiprintf("\rRTCP Header Validity Check failed!"BLANK_LINE);
		return 1;
	}
	if((ret=ssrc_check(rtp_sess, ntohl((pkt->r).sr.ssrc), &stm_src, server, RTCP)) == -1)
		return 1;
	else if ( ret == 1 ){
		if (pkt->common.pt == RTCP_SR)
			rtp_sess->sess_stats.senders++;
		rtp_sess->sess_stats.members++;
	}

	if((ret=parse_rtcp_pkt(stm_src, pkt, n)) != 0)
		return ret;
	else
		rtp_sess->sess_stats.avg_rtcp_size = n/16. + rtp_sess->sess_stats.avg_rtcp_size * 15./16.;
		
	return 0;
}
