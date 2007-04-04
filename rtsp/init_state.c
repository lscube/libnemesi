/* * 
 *  $Id:init_state.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/methods.h>

int init_state(rtsp_thread * rtsp_th, short event)
{
	switch (event) {
	case RTSP_GET_RESPONSE:
		if (handle_get_response(rtsp_th))	//{
			// close(rtsp_th->fd);
			// rtsp_th->fd=-1;
			return 1;
		// }
		// get_curr_sess(NULL, NULL, NULL);
		get_curr_sess(GCS_UNINIT);
		/*
		   if (get_curr_sess(rtsp_th, NULL, NULL))
		   return 1;
		 */
		get_curr_sess(GCS_INIT, rtsp_th);
		if (send_setup_request(rtsp_th))	// {
			// rtsp_reinit(rtsp_th);        
			return 1;
		// }
		break;

	case RTSP_SETUP_RESPONSE:
		if (handle_setup_response(rtsp_th))	// {
			// rtsp_reinit(rtsp_th);        
			return 1;
		// }
		// if (get_curr_sess(rtsp_th, NULL, NULL)) {
		if (!get_curr_sess(GCS_NXT_MED)) {
			/* Nessun altra SETUP da inviare */
			rtsp_th->rtp_th->rtp_sess_head =
			    rtsp_th->rtsp_queue->media_queue->rtp_sess;
			/* Esecuzione del Thread RTP: uno per ogni sessione RTSP */
			if (rtp_thread_create(rtsp_th->rtp_th))
				return nms_printf(NMSML_FATAL,
						  "Cannot create RTP Thread!\n");

			/* Esecuzione del Thread RTCP: uno per ogni sessione RTSP */
			if (rtcp_thread_create(rtsp_th->rtp_th))
				return nms_printf(NMSML_FATAL,
						  "Cannot create RTCP Thread!\n");

			rtsp_th->status = READY;
			rtsp_unbusy(rtsp_th);
			// rtsp_th->busy = 0;
			/* Inizializza a NULL le variabili statiche interne */
			// get_curr_sess(NULL, NULL, NULL);
			get_curr_sess(GCS_UNINIT);
			break;
		}
		if (send_setup_request(rtsp_th))
			return 1;
		break;
	default:
		nms_printf(NMSML_ERR,
			   "Could not handle method in INIT state\n");
		return 1;
		break;
	}
	return 0;
}
