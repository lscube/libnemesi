/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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
#include <nemesi/etui.h>

#include <fcntl.h>

void rtsp_clean(void *rtsp_thread)
{
	// struct RTSP_Ctrl *rtsp_ctrl = (struct RTSP_Ctrl *)rtsp_control;
	struct RTSP_Thread *rtsp_th = (struct RTSP_Thread *)rtsp_thread;
	int n;
#ifdef USE_UIPRINTF
	char optstr[256];
#endif // USE_UIPRINTF
	struct command *comm = rtsp_th->comm;
	int command_fd = rtsp_th->pipefd[0];
	char ch[1];

	if ( (n = fcntl(command_fd, F_GETFL, 0)) < 0 )
		nmsprintf(NMSML_ERR, "fcntl F_GETFL error\n");
	n |= O_NONBLOCK;
	 if (fcntl(command_fd, F_SETFL, n) < 0)
		 nmsprintf(NMSML_ERR, "fcntl F_SETFL error\n");
#if 1 // We must read last teardown reply from server
	nmsprintf(NMSML_DBG1, "Waiting for last Teardown response\n");
	if((n=read(command_fd, ch, 1)) == 1)
		if(cmd[comm->opcode] (rtsp_th, comm->arg))
			return;
	if( (*(rtsp_th->waiting_for)) && (rtsp_th->fd != -1) ) {
		if ((n=rtsp_recv(rtsp_th)) < 0)
			nmsprintf(NMSML_WARN, "No teardown response received\n");
		else if (n > 0){
			if (full_msg_rcvd(rtsp_th))
				/*if (*/handle_rtsp_pkt(rtsp_th);/*)*/
					/*nmsprintf(NMSML_ERR, "\nError!\n");*/
		} else
			nmsprintf(NMSML_ERR, "Server died prematurely!\n");
	}
#endif
	rtsp_reinit(rtsp_th);
	nmsprintf(NMSML_DBG1, "RTSP Thread R.I.P.\n");
#ifdef USE_UIPRINTF
	fprintf(stderr, "\r"); /* TODO Da ottimizzare */
	while((n=read(UIINPUT_FILENO, optstr, 1)) > 0)
		write(STDERR_FILENO, optstr, n);
#endif // USE_UIPRINTF

	close(rtsp_th->pipefd[0]);
	close(rtsp_th->pipefd[1]);
}
