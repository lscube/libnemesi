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

#include <nemesi/etui.h>

int seturlname(struct RTSP_Ctrl *rtsp_ctrl, char *urlname)
{
	char *server = NULL, *port = NULL, *path = NULL;
	// very very tmp
	struct RTSP_Thread *rtsp_th = (struct RTSP_Thread *)rtsp_ctrl;

	if (urltokenize(urlname, &server, &port, &path) > 0)
		return 1;
	if (port == NULL) {
		if ((port = (char *) malloc(sizeof(char) * 4)) == NULL)
			sprintf(port, "%d", RTSP_DEFAULT_PORT);
	}

	if ((rtsp_th->urlname =
	     (char *) malloc(sizeof(char) * (strlen("rtsp://") + strlen(server) + strlen(path) + 1))) == NULL)
		return 1;
	strcpy(rtsp_th->urlname, "rtsp://");
	strcat(rtsp_th->urlname, server);
	strcat(rtsp_th->urlname, path);
	if ((rtsp_th->server_port = (char *) malloc(sizeof(char) * (strlen(port) + 1))) == NULL)
		return 1;
	strcpy(rtsp_th->server_port, port);



	return 0;
}
