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

#include <nemesi/rtsp.h>

int reinit_rtsp(struct RTSP_Thread *rtsp_th)
{
	struct RTSP_Medium *med, *pmed;
	struct RTSP_Session *sess, *psess;
	void *ret;
	
	sess=psess=rtsp_th->rtsp_queue;
#if 1
	// check for active rtp/rtcp session
	if(sess && sess->media_queue && sess->media_queue->rtp_sess) {
		if(sess->media_queue->rtp_sess->rtcp_tid > 0){
			nmsprintf(2, "Sending cancel signal to RTCP Thread (ID: %d)\n", sess->media_queue->rtp_sess->rtcp_tid);
			if (pthread_cancel(sess->media_queue->rtp_sess->rtcp_tid) != 0)
				nmsprintf(3, "Error while sending cancelation to RTCP Thread.\n");
			else
				pthread_join(sess->media_queue->rtp_sess->rtcp_tid, (void **)&ret);
			if ( ret != PTHREAD_CANCELED )
				nmsprintf(3, "Warning! RTCP Thread joined, but  not canceled!\n");
		}
		if(sess->media_queue->rtp_sess->rtp_tid > 0){
			nmsprintf(2, "Sending cancel signal to RTP Thread (ID: %d)\n", sess->media_queue->rtp_sess->rtp_tid);
			if(pthread_cancel(sess->media_queue->rtp_sess->rtp_tid) != 0)
				nmsprintf(3, "Error while sending cancelation to RTP Thread.\n");
			else
				pthread_join(sess->media_queue->rtp_sess->rtp_tid, (void **)&ret);
			if( ret != PTHREAD_CANCELED)
				nmsprintf(3, "Warning! RTP Thread joined, but not canceled.\n");
		}
	}
#endif
	while(sess != NULL){
		sdp_session_destroy(sess->info); //!< free sdp description info
		free(sess->body);
		free(sess->content_base);
		for(med=sess->media_queue; med; pmed=med, med=med->next, free(pmed));
		/* like these
		med=pmed=sess->media_queue;
		while(med != NULL){
			pmed=med;
			med=med->next;
			free(pmed);
		}
		*/
		psess=sess;
		sess=sess->next;
		free(psess);
	}
#if 1
	free(rtsp_th->server_port);
	free(rtsp_th->urlname);
	free((rtsp_th->in_buffer).data);
	
	close(rtsp_th->fd);
	rtsp_th->fd = -1;
	rtsp_th->status = INIT;
	memset(rtsp_th->waiting_for, '\0', strlen(rtsp_th->waiting_for));
	rtsp_th->urlname = NULL;
	rtsp_th->server_port = NULL;
	(rtsp_th->in_buffer).size = 0;
	(rtsp_th->in_buffer).data = NULL;
	rtsp_th->rtsp_queue = NULL;
	rtsp_th->busy=0;
#endif

	return 0;
}
