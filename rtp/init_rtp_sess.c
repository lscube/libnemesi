/* * 
 *  $Id$
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

struct RTP_Session *init_rtp_sess(struct sockaddr localaddr, struct sockaddr peeraddr)
{
	struct RTP_Session *rtp_sess;

	if((rtp_sess=(struct RTP_Session *)calloc(1, sizeof(struct RTP_Session))) == NULL){
		uiprintf("Cannot allocate memory!\n");
		return NULL;
	}

	rtp_sess->rtp_tid=-1;
	rtp_sess->dec_tid=-1;	
	rtp_sess->rtpfd=-1;
	rtp_sess->rtcpfd=-1;
	rtp_sess->local_ssrc=random32(0);
	pthread_mutex_init(&rtp_sess->syn,NULL);
	if((rtp_sess->transport.spec=(char *)malloc(sizeof(char)*(strlen(RTP_AVP_UDP)+1))) ==NULL){
		uiprintf("Cannot allocate memory!\n");
		return NULL;
	}
	strcpy(rtp_sess->transport.spec, RTP_AVP_UDP);
	rtp_sess->transport.delivery=unicast;
	(rtp_sess->transport).srcaddr=((struct sockaddr_in *)&peeraddr)->sin_addr;
	(rtp_sess->transport).dstaddr=((struct sockaddr_in *)&localaddr)->sin_addr;
	rtp_sess->transport.mode=play;
	rtp_sess->transport.ssrc=rtp_sess->local_ssrc;
	
	rtp_sess->sess_stats.pmembers=1;
	rtp_sess->sess_stats.members=1;
	rtp_sess->sess_stats.initial=1;
	rtp_sess->sess_stats.avg_rtcp_size=200; /* RR + SDES ~= 200 Bytes */
	rtp_sess->sess_stats.rtcp_bw=BANDWIDTH;

	return rtp_sess;
}
