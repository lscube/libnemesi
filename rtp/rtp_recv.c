/* * 
 *  ./rtp/rtp_recv.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:13 $
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
#include <nemesi/rtpptdefs.h>

int rtp_recv(struct RTP_Session *rtp_sess)
{

	int n, ret;
	uint32 rate;
	int slot;
	rtp_pkt *pkt;
	struct Stream_Source *stm_src;
	struct timeval now;
	uint32 transit;
	int32 delta;

	struct sockaddr_in server;
	socklen_t server_len=sizeof(struct sockaddr_in);

	if( (slot=bpget(&(rtp_sess->bp))) < 0){
		uiprintf("No more space in Playout Buffer!"BLANK_LINE);
		return 1;
	}
	
	n=recvfrom(rtp_sess->rtpfd, &((rtp_sess->bp).bufferpool[slot]), BP_SLOT_SIZE, 0, (struct sockaddr *)&server, &server_len);
	gettimeofday(&now, NULL);

	pkt=(rtp_pkt *)&((rtp_sess->bp).bufferpool[slot]);

	if ( rtp_hdr_val_chk(pkt, n) ){
		uiprintf("RTP header validity check FAILED!\n");
		bpfree(&(rtp_sess->bp), slot);
		return 1;
	}

	if((ret=ssrc_check(rtp_sess, ntohl(pkt->ssrc), &stm_src, server, RTP)) == -1){
		return 1;
	} else if (ret == 2){
		bprmv(&(rtp_sess->bp), &(stm_src->po), slot);
		return 0;
	} else if ( ret == 1){
		(stm_src->ssrc_stats).probation=MIN_SEQUENTIAL;
		(stm_src->ssrc_stats).max_seq = ntohs(pkt->seq) - 1;
		
		if ( (rate=(rtp_pt_defs[pkt->pt].rate)) == 0 )
			rate=RTP_DEF_CLK_RATE;
		(stm_src->ssrc_stats).transit=(uint32)(((double)now.tv_sec + (double)now.tv_usec/1000000.0)*(double)rate) - ntohl(pkt->time);
		(stm_src->ssrc_stats).jitter=0;
		(stm_src->ssrc_stats).firstts=ntohl(pkt->time);
		(stm_src->ssrc_stats).firsttv=now;
		init_seq(stm_src, ntohs(pkt->seq));
		rtp_sess->sess_stats.senders++;
		rtp_sess->sess_stats.members++;

	} else if (ret == 0){
		update_seq(stm_src, ntohs(pkt->seq));
		
		if ( (rate=(rtp_pt_defs[pkt->pt].rate)) == 0 )
			rate=RTP_DEF_CLK_RATE;

		transit = (uint32)(((double)now.tv_sec + (double)now.tv_usec/1000000.0)*(double)rate) - ntohl(pkt->time);
		delta = transit - stm_src->ssrc_stats.transit;
		stm_src->ssrc_stats.transit = transit;
		if ( delta < 0)
			delta = -delta;
		stm_src->ssrc_stats.jitter += (1./16.)*((double)delta - stm_src->ssrc_stats.jitter);
	}

	if((ret=poadd(&(stm_src->po), slot, (stm_src->ssrc_stats).cycles)) == 1){
		uiprintf("\nWARNING: Duplicate pkt found... discarded\n");
		bpfree(&(rtp_sess->bp), slot);
		return 0;
	} else if (ret == 2)
		uiprintf("\nWARNING: Misordered pkt found... reordered\n");

	((stm_src->po).pobuff[slot]).pktlen=n;
	
	return 0;
}
