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

/*!
 * \brief Function called to start rtsp lib.
 *
 * This function allocate structures and comunication pipe for reading commands
 * from user interface, then starts RTSP thread.
 *
 * \return RTSP_Ctrl structure. This is the interface structure between user
 * interface and RTSP librarween user interface and RTSP library.
 */
struct RTSP_Ctrl *init_rtsp(void)
{
	struct RTSP_Thread *rtsp_th;
	pthread_attr_t rtsp_attr;
	pthread_mutexattr_t mutex_attr;
	int n;

	// if ( !(rtsp_th = (struct RTSP_Thread *) malloc(sizeof(struct RTSP_Thread))) )
	// We use calloc so that we are not in need to initialize to zero below
	if ( !(rtsp_th = (struct RTSP_Thread *) calloc(1, sizeof(struct RTSP_Thread))) )
		return NULL;

	if (pipe(rtsp_th->pipefd) < 0)
		return NULL;

	if ((n = pthread_mutexattr_init(&mutex_attr)) > 0)
		return NULL;
#if 0
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if ((n = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED)) > 0)
		return NULL;
#endif
#endif
	if ((n = pthread_mutex_init(&(rtsp_th->comm_mutex), &mutex_attr)) > 0)
		return NULL;
	
	if ((rtsp_th->comm = (struct command *) malloc(sizeof(struct command))) == NULL)
		return NULL;

	rtsp_th->fd = -1;
	rtsp_th->status = INIT;
#if 0 // we do not need to initilize to zero because of calloc usage
	rtsp_th->descr_fmt = 0;
	memset(rtsp_th->waiting_for, '\0', sizeof(rtsp_th->waiting_for));
	rtsp_th->busy=0;
	rtsp_th->urlname = NULL;
	(rtsp_th->in_buffer).size = 0;
	(rtsp_th->in_buffer).first_pkt_size = 0;
	(rtsp_th->in_buffer).data = NULL;
	rtsp_th->rtsp_queue = NULL;
#endif

	cmd[0] = open_cmd;
	cmd[1] = play_cmd;
	cmd[2] = pause_cmd;
	cmd[3] = stop_cmd;
	cmd[4] = close_cmd;

	state_machine[0] = init_state;
	state_machine[1] = ready_state;
	state_machine[2] = playing_state;
	state_machine[3] = recording_state;
	
	// Creation of RTSP Thread
	pthread_attr_init(&rtsp_attr);
	if (pthread_attr_setdetachstate(&rtsp_attr, PTHREAD_CREATE_JOINABLE) != 0) {
		nmsprintf(NMSML_FATAL, "Cannot set RTSP Thread attributes!\n");
		return NULL;
	}

	if ((n = pthread_create(&rtsp_th->rtsp_tid, NULL, &rtsp, (void *) rtsp_th)) > 0) {
		nmsprintf(NMSML_FATAL, "Cannot create RTSP Thread: %s\n", strerror(n));
		return NULL;
	}


	return (struct RTSP_Ctrl *)rtsp_th;
}
