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
#include <nemesi/methods.h>

int send_setup_request(struct RTSP_Thread *rtsp_th)
{

	char b[256];
	char *options = NULL;
	struct RTSP_Session *rtsp_sess;
	struct RTSP_Medium *rtsp_med;
	struct sockaddr rtpaddr, rtcpaddr;
	socklen_t rtplen=sizeof(struct sockaddr), rtcplen=sizeof(struct sockaddr);
	unsigned int rnd;

	memset(b, 0, 256);

	// if ( get_curr_sess(NULL, &rtsp_sess, &rtsp_med))
	if ( !(rtsp_sess=get_curr_sess(GCS_CUR_SESS)) || !(rtsp_med=get_curr_sess(GCS_CUR_MED)) )
		return 1;

	rnd=(rand()%((2<<15) - 1 - 5001))+5001;

	if ( (rnd%2) )
		rnd++;
	
	sprintf(b, "%d", rnd);
	server_create(NULL, b, &(rtsp_med->rtp_sess->rtpfd));
	
	sprintf(b, "%d", rnd+1);
	server_create(NULL, b, &(rtsp_med->rtp_sess->rtcpfd));

	/* per sapere il numero di porta assegnato */
	getsockname(rtsp_med->rtp_sess->rtpfd, &rtpaddr, &rtplen);
	getsockname(rtsp_med->rtp_sess->rtcpfd, &rtcpaddr, &rtcplen);

	rtsp_med->rtp_sess->transport.cli_ports[0]=((struct sockaddr_in *)&rtpaddr)->sin_port;
	rtsp_med->rtp_sess->transport.cli_ports[1]=((struct sockaddr_in *)&rtcpaddr)->sin_port;

	if ( set_transport_str(rtsp_med->rtp_sess, &options))
		return 1;

	if ( rtsp_sess->content_base != NULL)
		sprintf(b, "%s %s/%s %s\n", SETUP_TKN, rtsp_sess->content_base, rtsp_med->filename, RTSP_VER);
	else
		sprintf(b, "%s %s %s\n", SETUP_TKN, rtsp_med->filename, RTSP_VER);
	sprintf(b + strlen(b), "CSeq: %d\n", ++(rtsp_sess->CSeq));
	sprintf(b + strlen(b), "Transport: %s\n", options);
	
	if (rtsp_sess->Session_ID) //Caso di controllo aggregato: è già stato definito un numero per la sessione corrente.
		sprintf(b + strlen(b), "Session: %llu\n", rtsp_sess->Session_ID);
	
	strcat(b, "\r\n"); 

	if (!tcp_write(rtsp_th->fd, b, strlen(b))) {
		nmsprintf(NMSML_ERR, "Cannot send SETUP request...\n");
		return 1;
	}

	sprintf(rtsp_th->waiting_for, "%d.%d", RTSP_SETUP_RESPONSE, rtsp_sess->CSeq);

	free(options);

	return 0;
}
