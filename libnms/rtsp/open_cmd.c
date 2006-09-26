/* * 
 *  $Id:open_cmd.c 267 2006-01-12 17:19:45Z shawill $
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
#include <stdarg.h>

/**
* funzione che gestisce il comando open da parte della UI.
* Si occupa di inizializzare la sessione e di mettere il thread nello stato READY.
* @param rtap_t il puntatore che conterra la nuova struttura rtsp_thread.
* */
int open_cmd(rtsp_thread * rtsp_th, ...)
{
	char *server;

	if (rtsp_th->status != INIT) {
		nms_printf(NMSML_WARN, "Client already connected!\n");
		return 1;
	}
	if (!*rtsp_th->comm->arg) {
		nms_printf(NMSML_ERR, "No address given\n");
		return 1;
	}
	if (seturlname(rtsp_th, rtsp_th->comm->arg) > 0)
		return 1;
	urltokenize(rtsp_th->urlname, &server, NULL, NULL);
	if (server_connect(server, rtsp_th->server_port, &rtsp_th->transport.fd, TCP)) {
		rtsp_th->transport.fd = -1;
		return nms_printf(NMSML_ERR, "Cannot connect to the server\n");
	}
	free(server);
	if (send_get_request(rtsp_th))
		return 1;

	return 0;
}
