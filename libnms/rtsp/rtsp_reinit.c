/* * 
 *  $Id:rtsp_reinit.c 267 2006-01-12 17:19:45Z shawill $
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

int rtsp_reinit(rtsp_thread * rtsp_th)
{
	rtsp_medium *med, *pmed;
	rtsp_session *sess, *psess;
	void *ret;

	if (!(sess = psess = rtsp_th->rtsp_queue))
		return 0;
#if 1				// TODO: fix last teardown response wait
	// check for active rtp/rtcp session
	if (sess->media_queue && sess->media_queue->rtp_sess) {
		if (rtsp_th->rtp_th->rtcp_tid > 0) {
			nms_printf(NMSML_DBG1, "Sending cancel signal to RTCP Thread (ID: %lu)\n",
				   rtsp_th->rtp_th->rtcp_tid);
			if (pthread_cancel(rtsp_th->rtp_th->rtcp_tid) != 0)
				nms_printf(NMSML_DBG2, "Error while sending cancelation to RTCP Thread.\n");
			else
				pthread_join(rtsp_th->rtp_th->rtcp_tid, (void **) &ret);
			if (ret != PTHREAD_CANCELED)
				nms_printf(NMSML_DBG2, "Warning! RTCP Thread joined, but  not canceled!\n");
			rtsp_th->rtp_th->rtcp_tid = 0;
		}
		if (rtsp_th->rtp_th->rtp_tid > 0) {
			nms_printf(NMSML_DBG1, "Sending cancel signal to RTP Thread (ID: %lu)\n",
				   rtsp_th->rtp_th->rtp_tid);
			if (pthread_cancel(rtsp_th->rtp_th->rtp_tid) != 0)
				nms_printf(NMSML_DBG2, "Error while sending cancelation to RTP Thread.\n");
			else
				pthread_join(rtsp_th->rtp_th->rtp_tid, (void **) &ret);
			if (ret != PTHREAD_CANCELED)
				nms_printf(NMSML_DBG2, "Warning! RTP Thread joined, but not canceled.\n");
			rtsp_th->rtp_th->rtp_tid = 0;
		}
	}
#endif
	// the destruction of sdp informations must be done only once, because
	// in all other sessions the pointer is the same and the allocated
	// struct is one
	sdp_session_destroy(sess->info);	//!< free sdp description info
	while (sess) {
		// MUST be done only once
		// sdp_session_destroy(sess->info); //!< free sdp description info
		free(sess->body);
		free(sess->content_base);
		for (med = sess->media_queue; med; pmed = med, med = med->next, free(pmed));
		/* like these
		   med=pmed=sess->media_queue;
		   while(med != NULL){
		   pmed=med;
		   med=med->next;
		   free(pmed);
		   }
		 */
		psess = sess;
		sess = sess->next;
		free(psess);
	}

	free(rtsp_th->server_port);
	free(rtsp_th->urlname);
	free((rtsp_th->in_buffer).data);

	nmst_close(&rtsp_th->transport);
	nmst_init(&rtsp_th->transport);
	rtsp_th->status = INIT;
	memset(rtsp_th->waiting_for, '\0', strlen(rtsp_th->waiting_for));
	rtsp_th->urlname = NULL;
	rtsp_th->server_port = NULL;
	(rtsp_th->in_buffer).size = 0;
	(rtsp_th->in_buffer).data = NULL;
	rtsp_th->rtsp_queue = NULL;
	if (rtsp_th->comm->opcode == NONE)
		rtsp_th->busy = 0;

	// reset first RP port
	if (rtsp_th->hints
	    || ((rtsp_th->hints->first_rtp_port > RTSP_MIN_RTP_PORT) && (rtsp_th->hints->first_rtp_port < 65535))) {
		rtsp_th->force_rtp_port = rtsp_th->hints->first_rtp_port;
		if (rtsp_th->force_rtp_port % 2)
			rtsp_th->force_rtp_port++;
	} else
		rtsp_th->force_rtp_port = 0;

	return 0;
}
