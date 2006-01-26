/* * 
 *  $Id:rtsp_init.c 267 2006-01-12 17:19:45Z shawill $
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

#define RET_ERR(ret_level, ...) { \
					nms_printf(ret_level, __VA_ARGS__ ); \
					free(rtsp_th->comm); \
					free(rtsp_th); \
					return NULL; \
				}

/*!
 * \brief Function called to start rtsp lib.
 *
 * This function allocate structures and comunication pipe for reading commands
 * from user interface, then starts RTSP thread.
 *
 * \return rtsp_ctrl structure. This is the interface structure between user
 * interface and RTSP librarween user interface and RTSP library.
 */
struct rtsp_ctrl *rtsp_init(nms_rtsp_hints *hints)
{
	struct rtsp_thread *rtsp_th;
	pthread_attr_t rtsp_attr;
	pthread_mutexattr_t mutex_attr;
	// pthread_condattr_t cond_attr;
	int n;

	// if ( !(rtsp_th = (struct rtsp_thread *) malloc(sizeof(struct rtsp_thread))) )
	// We use calloc so that we are not in need to initialize to zero below
	if ( !(rtsp_th = (struct rtsp_thread *) calloc(1, sizeof(struct rtsp_thread))) ) {
		nms_printf(NMSML_FATAL, "Could not alloc memory!\n");
		return NULL;
	}

	/* not needed if we use calloc
	 * rtsp_th->comm = NULL;
	 */

	if (pipe(rtsp_th->pipefd) < 0)
		RET_ERR(NMSML_FATAL, "Could not create pipe\n")

	if ((n = pthread_mutexattr_init(&mutex_attr)) > 0)
		RET_ERR(NMSML_FATAL, "Could not init mutex attributes\n")
#if 0
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if ((n = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED)) > 0)
		return NULL;
#endif
#endif
	if ((n = pthread_mutex_init(&(rtsp_th->comm_mutex), &mutex_attr)) > 0)
		RET_ERR(NMSML_FATAL, "Could not init mutex\n")

	/* // we give NULL to cond_init: uncommet this (and the declaration above)
	 * if you want to give different attributes to cond
	if (pthread_condattr_init(&cond_attr) > 0)
		RET_ERR(NMSML_FATAL, "Could not init condition variable attributes\n")
	*/
	if (pthread_cond_init(&(rtsp_th->cond_busy), NULL /*&cond_attr*/) > 0)
		RET_ERR(NMSML_FATAL, "Could not init condition variable\n")
	
	if ((rtsp_th->comm = (struct command *) malloc(sizeof(struct command))) == NULL)
		RET_ERR(NMSML_FATAL, "Could not alloc memory\n")

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
	rtsp_th->force_rtp_port = 0;
#endif

	rtsp_th->hints = hints;
	// check for the exactness of values hinted
	if (hints) { // hints given
		// set first RTP port
		if (hints->first_rtp_port > 0) {
			if (hints->first_rtp_port < RTSP_MIN_RTP_PORT)
				RET_ERR(NMSML_ERR, "For security reasons RTSP Library imposes that port number should be greater than %d\n", RTSP_MIN_RTP_PORT)
			else if (hints->first_rtp_port > 65535)
				RET_ERR(NMSML_ERR, "Port number can't be greater than 65535\n")
			rtsp_th->force_rtp_port = hints->first_rtp_port;
			nms_printf(NMSML_WARN, "RTP ports forced by user (not randomly generated)\n");
		}
	}
	
	// hook to rtp lib
	if ( !(rtsp_th->rtp_th = rtp_init()) )
		RET_ERR(NMSML_ERR, "Cannot initialize RTP structs\n")

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
	if (pthread_attr_setdetachstate(&rtsp_attr, PTHREAD_CREATE_JOINABLE) != 0)
		RET_ERR(NMSML_FATAL, "Cannot set RTSP Thread attributes!\n")

	if ((n = pthread_create(&rtsp_th->rtsp_tid, NULL, &rtsp, (void *) rtsp_th)) > 0)
		RET_ERR(NMSML_FATAL, "Cannot create RTSP Thread: %s\n", strerror(n))

	return (struct rtsp_ctrl *)rtsp_th;
}
