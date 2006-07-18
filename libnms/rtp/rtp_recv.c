/* * 
 *  $Id:rtp_recv.c 267 2006-01-12 17:19:45Z shawill $
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

int rtp_recv(rtp_session *rtp_sess)
{

	int n;
	uint32 rate;
	int slot;
	rtp_pkt *pkt;
	rtp_ssrc *stm_src;
	struct timeval now;
	uint32 transit;
	int32 delta;

	struct sockaddr_storage serveraddr;
	nms_sockaddr server = { (struct sockaddr *)&serveraddr, sizeof(serveraddr) };

	if( (slot=bpget(&(rtp_sess->bp))) < 0) {
		nms_printf(NMSML_VERB, "No more space in Playout Buffer!"BLANK_LINE);
		return 1;
	}
	
	if ( (n=recvfrom(rtp_sess->rtpfd, &((rtp_sess->bp).bufferpool[slot]), BP_SLOT_SIZE, 0, server.addr, &server.addr_len)) == -1 ) {
		switch (errno) {
			case EBADF:
				nms_printf(NMSML_ERR, "RTP recvfrom: invalid descriptor\n");
				break;
			case ENOTSOCK:
				nms_printf(NMSML_ERR, "RTP recvfrom: not a socket\n");
				break;
			case EINTR:
				nms_printf(NMSML_ERR, "RTP recvfrom: The receive was interrupted by delivery of a signal\n");
				break;
			case EFAULT:
				nms_printf(NMSML_ERR, "RTP recvfrom: The buffer points outside userspace\n");
				break;
			case EINVAL:
				nms_printf(NMSML_ERR, "RTP recvfrom: Invalid argument passed.\n");
				break;
			default:
				nms_printf(NMSML_ERR, "in RTP recvfrom\n");
				break;
		}
		return 1;
	}
	gettimeofday(&now, NULL);

	pkt=(rtp_pkt *)&((rtp_sess->bp).bufferpool[slot]);

	if ( rtp_hdr_val_chk(pkt, n) ) {
		nms_printf(NMSML_NORM, "RTP header validity check FAILED!\n");
		bpfree(&(rtp_sess->bp), slot);
		return 0;
	}

	switch ( rtp_ssrc_check(rtp_sess, RTP_PKT_SSRC(pkt), &stm_src, &server, RTP) ) {
		case SSRC_KNOWN:
			rtp_update_seq(stm_src, RTP_PKT_SEQ(pkt));
			
			if ( !rtp_sess->ptdefs[pkt->pt] || !(rate=(rtp_sess->ptdefs[pkt->pt]->rate)) )
				rate=RTP_DEF_CLK_RATE;
	
			transit = (uint32)(((double)now.tv_sec + (double)now.tv_usec/1000000.0)*(double)rate) - ntohl(pkt->time);
			delta = transit - stm_src->ssrc_stats.transit;
			stm_src->ssrc_stats.transit = transit;
			if ( delta < 0)
				delta = -delta;
			stm_src->ssrc_stats.jitter += (1./16.)*((double)delta - stm_src->ssrc_stats.jitter);
			break;
		case SSRC_NEW:
			rtp_sess->sess_stats.senders++;
			rtp_sess->sess_stats.members++;
		case SSRC_RTPNEW:
			(stm_src->ssrc_stats).probation=MIN_SEQUENTIAL;
			(stm_src->ssrc_stats).max_seq = RTP_PKT_SEQ(pkt) - 1;
			
			if ( !rtp_sess->ptdefs[pkt->pt] || !(rate=(rtp_sess->ptdefs[pkt->pt]->rate)) )
				rate=RTP_DEF_CLK_RATE;
			(stm_src->ssrc_stats).transit=(uint32)(((double)now.tv_sec + (double)now.tv_usec/1000000.0)*(double)rate) - ntohl(pkt->time);
			(stm_src->ssrc_stats).jitter=0;
			(stm_src->ssrc_stats).firstts=ntohl(pkt->time);
			(stm_src->ssrc_stats).firsttv=now;
			// rtp_init_seq(stm_src, RTP_PKT_SEQ(pkt));
			rtp_update_seq(stm_src, RTP_PKT_SEQ(pkt));
			break;
		case SSRC_COLLISION:
			bprmv(&(rtp_sess->bp), &(stm_src->po), slot);
			return 0;
			break;
		case -1:
			return 1;
			break;
		default:
			break;
	}

	switch ( poadd(&(stm_src->po), slot, (stm_src->ssrc_stats).cycles) ) {
		case PKT_DUPLICATED:
			nms_printf(NMSML_VERB, "WARNING: Duplicate pkt found... discarded\n");
			bpfree(&(rtp_sess->bp), slot);
			return 0;
			break;
		case PKT_MISORDERED:
			nms_printf(NMSML_VERB, "WARNING: Misordered pkt found... reordered\n");
			break;
		default:
			break;
	}

	((stm_src->po).pobuff[slot]).pktlen=n;
	
	return 0;
}
