/* * 
 *  $Id:send_pause_request.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/wsocket.h>

int send_pause_request(rtsp_thread *rtsp_th, char *range)
{
	char b[256];
	rtsp_session *rtsp_sess;

	// get_curr_sess(NULL, &rtsp_sess, NULL);
	rtsp_sess=get_curr_sess(GCS_CUR_SESS);
	
	if ( rtsp_sess->content_base != NULL)
		if (*(rtsp_sess->pathname) != 0)
			sprintf(b, "%s %s/%s %s"RTSP_EL"CSeq: %d"RTSP_EL, PAUSE_TKN, rtsp_sess->content_base, rtsp_sess->pathname, RTSP_VER, ++(rtsp_sess->CSeq));
		else
			sprintf(b, "%s %s %s"RTSP_EL"CSeq: %d"RTSP_EL, PAUSE_TKN, rtsp_sess->content_base, RTSP_VER, ++(rtsp_sess->CSeq));
	else
		sprintf(b, "%s %s %s"RTSP_EL"CSeq: %d"RTSP_EL, PAUSE_TKN, rtsp_sess->pathname, RTSP_VER, ++(rtsp_sess->CSeq));
		
	if (rtsp_sess->Session_ID != 0)	/* must add session ID? */
		sprintf(b + strlen(b), "Session: %llu"RTSP_EL, rtsp_sess->Session_ID);
	if (range && *range)
		sprintf(b + strlen(b), "Range: %s"RTSP_EL, range);
	else
		sprintf(b + strlen(b), "Range: time=0-"RTSP_EL);
	
	strcat(b, RTSP_EL);

	if (!nmst_write(&rtsp_th->transport, b, strlen(b))) {
		nms_printf(NMSML_ERR, "Cannot send PAUSE request...\n");
		return 1;
	}

	sprintf(rtsp_th->waiting_for, "%d:%llu.%d", RTSP_PAUSE_RESPONSE, rtsp_sess->Session_ID, rtsp_sess->CSeq);

	return 0;
}
