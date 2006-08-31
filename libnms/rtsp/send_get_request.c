/* * 
 *  $Id:send_get_request.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/version.h>
#include <nemesi/rtsp.h>
#include <nemesi/methods.h>
#include <nemesi/wsocket.h>

int send_get_request(rtsp_thread *rtsp_th)
{
	char b[256];

	/* save the url string for future use in setup request. */
	sprintf(b, "%s %s %s"RTSP_EL"CSeq: %d"RTSP_EL, GET_TKN, rtsp_th->urlname, RTSP_VER, 1);
	strcat(b, "Accept: application/sdp;"RTSP_EL);	/* application/x-rtsp-mh"RTSP_EL); */
	sprintf(b + strlen(b), "User-Agent: %s - %s -- Release %s (%s)"RTSP_EL, PROG_NAME, PROG_DESCR, VERSION,
		VERSION_NAME);
	strcat(b, RTSP_EL);
	if (!nmst_write(&rtsp_th->transport, b, strlen(b))) {
		nms_printf(NMSML_ERR, "Cannot send DESCRIBE request...\n");
		return 1;
	}
	sprintf(rtsp_th->waiting_for, "%d", RTSP_GET_RESPONSE);

	return 0;
}
