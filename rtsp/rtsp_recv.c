/* * 
 *  ./rtsp/rtsp_recv.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:49 $
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

#ifndef BUFFERSIZE
#define BUFFERSIZE 163840
#endif
int rtsp_recv(struct RTSP_Thread *rtsp_th)
{
	int n;
	char buffer[BUFFERSIZE];

	memset(buffer, '\0', BUFFERSIZE);

	if ((n = tcp_read(rtsp_th->fd, buffer, BUFFERSIZE)) < 0) {
		uiprintf("ERROR reading from TCP socket\n");
		return n;
	}
	
	if (((rtsp_th->in_buffer).size) == 0) {
		if (((rtsp_th->in_buffer).data = (char *) malloc((n + 1) * sizeof(char))) == NULL) {
			uiprintf("Cannot alloc memory space for received RTSP data\n");
			return -1;
		}
		strcpy((rtsp_th->in_buffer).data, buffer);
	} else {
		if (((rtsp_th->in_buffer).data = (char *) realloc((rtsp_th->in_buffer).data,
								  (n + (rtsp_th->in_buffer).size +
								   1) * sizeof(char))) == NULL) {
			uiprintf("Cannot alloc memory space for received RTSP data\n");
			return -1;
		}
		strcat((rtsp_th->in_buffer).data, buffer);
	}
	(rtsp_th->in_buffer).size += n;
	(rtsp_th->in_buffer).data[(rtsp_th->in_buffer).size]='\0';
	return n;
}
