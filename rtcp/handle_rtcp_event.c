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

#include <nemesi/rtcp.h>

struct RTCP_Event *handle_rtcp_event(struct RTCP_Event *event)
{

	double t;
	struct timeval tv, now;
	struct RTP_Session *rtp_save;
	int n;
	
	gettimeofday(&now, NULL);
	
	switch( event->type ){

		case RTCP_RR: case RTCP_SDES:
			
			if (event->rtp_sess->ssrc_queue){ 
				n=send_rtcp_rr(event->rtp_sess);
				event->rtp_sess->sess_stats.avg_rtcp_size = (1./16.)*n + (15./16.)*(event->rtp_sess->sess_stats.avg_rtcp_size);
			}
			event->rtp_sess->sess_stats.tp=now;

			t=rtcp_interval(event->rtp_sess->sess_stats.members, \
				event->rtp_sess->sess_stats.senders, \
				event->rtp_sess->sess_stats.rtcp_bw, \
				event->rtp_sess->sess_stats.we_sent, \
				event->rtp_sess->sess_stats.avg_rtcp_size, \
				event->rtp_sess->sess_stats.initial);

			tv.tv_sec=(long int)t;
			tv.tv_usec=(long int)((t-tv.tv_sec)*1000000);
			timeval_add(&(event->rtp_sess->sess_stats.tn), &now, &tv);
		
			event->rtp_sess->sess_stats.initial=0;
			event->rtp_sess->sess_stats.pmembers=event->rtp_sess->sess_stats.members;
			
			rtp_save=event->rtp_sess;
			event=rtcp_deschedule(event);
			if ( (event=rtcp_schedule(event, rtp_save, rtp_save->sess_stats.tn, RTCP_RR)) == NULL)
				return NULL;

			break;

		case RTCP_BYE:
			send_rtcp_bye(event->rtp_sess);
			break;
		default:
			nmsprintf(2, "RTCP Event not handled!\n");
			break;
	}
	return event;
}
