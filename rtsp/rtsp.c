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

#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>

#include <nemesi/rtsp.h>

/**
* funzione che implementa il thread rtsp.
* Si mette in attesa di comandi dalla UI e li gestisce.
* Il funzionamento e' quello di una macchina a stati.
* */
void *rtsp(void *rtsp_args)
{
	struct RTSP_Thread *rtsp_th = ((struct RTSP_args *) rtsp_args)->rtsp_th;
	struct command *comm = ((struct RTSP_args *) rtsp_args)->comm;
	int command_fd = ((struct RTSP_args *) rtsp_args)->pipefd[0];
	fd_set readset;
	char ch[1];
	int n;
	
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	pthread_cleanup_push(rtsp_clean, (void *)rtsp_args);
	
	if (init_rtsp())
		pthread_exit(NULL);;

	while (1) {
		FD_ZERO(&readset);

		FD_SET(command_fd, &readset);
		if (rtsp_th->fd != -1)
			FD_SET(rtsp_th->fd, &readset);
		if (select(max(rtsp_th->fd, command_fd) + 1 , &readset, NULL, NULL, NULL) < 0){
			nmserror("(%s) %s\n", PROG_NAME, strerror(errno));
			pthread_exit(NULL);
		}
		if (rtsp_th->fd != -1)
			if (FD_ISSET(rtsp_th->fd, &readset)) {
				if ((n=rtsp_recv(rtsp_th)) < 0)
					pthread_exit(NULL);
				else if (n > 0){
					if (full_msg_rcvd(rtsp_th))
						if (handle_rtsp_pkt(rtsp_th))
							nmsprintf(1, "\nError!\n");
				} else {
					nmsprintf(1, "\nServer died prematurely!\n");
					rtsp_th->busy=0;
					nmsprintf(1, "Session closed.\n");
					reinit_rtsp(rtsp_th);
				}
			}
		if (FD_ISSET(command_fd, &readset)) {
			pthread_mutex_lock(&(((struct RTSP_args *) rtsp_args)->comm_mutex));
			read(command_fd, ch, 1);
			if (cmd[comm->opcode] (rtsp_th, comm->arg)) 
				nmsprintf(1, "Error handling user command.\n\n");
			pthread_mutex_unlock(&(((struct RTSP_args *) rtsp_args)->comm_mutex));
		}
	}
	
	pthread_cleanup_pop(1);
/*	return NULL; */
}
