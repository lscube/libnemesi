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

#include <nemesi/main.h>

int init_rtsp_args(struct RTSP_args **rtsp_args)
{

	pthread_mutexattr_t mutex_attr;
	int n;

	if (((*rtsp_args) = (struct RTSP_args *) malloc(sizeof(struct RTSP_args))) == NULL)
		return 1;
	memset(*rtsp_args, 0, sizeof(struct RTSP_args));
	if (pipe((*rtsp_args)->pipefd) < 0)
		return errno;

	if ((n = pthread_mutexattr_init(&mutex_attr)) > 0)
		return n;
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if ((n = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED)) > 0)
		return n;
#endif
	if ((n = pthread_mutex_init(&((*rtsp_args)->comm_mutex), &mutex_attr)) > 0)
		return n;
	
	if (((*rtsp_args)->comm = (struct command *) malloc(sizeof(struct command))) == NULL)
		return 1;

	if (((*rtsp_args)->rtsp_th = (struct RTSP_Thread *) malloc(sizeof(struct RTSP_Thread))) == NULL)
		return 1;
	(*rtsp_args)->rtsp_th->fd = -1;
	(*rtsp_args)->rtsp_th->status = INIT;
	memset((*rtsp_args)->rtsp_th->waiting_for, '\0', strlen((*rtsp_args)->rtsp_th->waiting_for));
	(*rtsp_args)->rtsp_th->busy=0;
	(*rtsp_args)->rtsp_th->urlname = NULL;
	((*rtsp_args)->rtsp_th->in_buffer).size = 0;
	((*rtsp_args)->rtsp_th->in_buffer).first_pkt_size = 0;
	((*rtsp_args)->rtsp_th->in_buffer).data = NULL;
	(*rtsp_args)->rtsp_th->rtsp_queue = NULL;

	return 0;

}
