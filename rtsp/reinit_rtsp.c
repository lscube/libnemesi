/* * 
 *  ./rtsp/reinit_rtsp.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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
	
	psess=sess=rtsp_th->rtsp_queue;
#if 1
	if(psess) {
		if(psess->media_queue->rtp_sess->rtcp_tid > 0){
			uiprintf("Sending cancel signal to RTCP Thread (ID: %d)\n", psess->media_queue->rtp_sess->rtcp_tid);
			if (pthread_cancel(psess->media_queue->rtp_sess->rtcp_tid) != 0)
				uiprintf("Error while sending cancelation to RTCP Thread.\n");
			else
				pthread_join(psess->media_queue->rtp_sess->rtcp_tid, (void **)&ret);
			if ( ret != PTHREAD_CANCELED )
				uiprintf("Warning! RTCP Thread joined, but  not canceled!\n");
		}
		if(psess->media_queue->rtp_sess->rtp_tid > 0){
			uiprintf("Sending cancel signal to RTP Thread (ID: %d)\n", psess->media_queue->rtp_sess->rtp_tid);
			if(pthread_cancel(psess->media_queue->rtp_sess->rtp_tid) != 0)
				uiprintf("Error while sending cancelation to RTP Thread.\n");
			else
				pthread_join(psess->media_queue->rtp_sess->rtp_tid, (void **)&ret);
			if( ret != PTHREAD_CANCELED)
				uiprintf("Warning! RTP Thread joined, but not canceled.\n");
		}
	}
#endif
	while(psess != NULL){
		free(psess->body);
		free(psess->content_base);
		pmed=med=psess->media_queue;
		while(pmed != NULL){
			med=pmed->next;
#if 0
			if(pmed->rtp_sess->rtcp_tid > 0){
				if (pthread_cancel(pmed->rtp_sess->rtcp_tid) != 0)
					uiprintf("Error while sending cancelation to RTCP Thread.\n");
				else 
					pthread_join(pmed->rtp_sess->rtcp_tid, (void **)&ret);
				if ( ret != PTHREAD_CANCELED )
					uiprintf("Warning! RTCP Thread joined, but  not canceled!\n");
			}
			if(pmed->rtp_sess->rtp_tid > 0){
				if(pthread_cancel(pmed->rtp_sess->rtp_tid) != 0)
					uiprintf("Error while sending cancelation to RTP Thread.\n");
				else 
					pthread_join(pmed->rtp_sess->rtp_tid, (void **)&ret);
				if( ret != PTHREAD_CANCELED)
					uiprintf("Warning! RTP Thread joined, but not canceled.\n");
			}
#endif
			free(pmed);
			pmed=med;
		}
		sess=psess->next;
		free(psess);
		psess=sess;
	}
/*	uiprintf("RTP THREAD cancelled\n"); */
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
#endif

	return 0;
}
