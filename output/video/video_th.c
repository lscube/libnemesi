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

#include <sys/select.h>
#include <sys/time.h>

#include <nemesi/video.h>
#include <nemesi/comm.h>
#include <nemesi/utils.h>

#define SLEEP_MS 40
#define DEF_FPS 25.0 // must be float

void *video_th(void *vc)
{
	NMSVideo *videoc = (NMSVideo *)vc;
	NMSVFunctions *funcs = videoc->functions;
	struct timeval tvsleep, tvstart, tvstop;
	float fps=DEF_FPS;

	// pthread cancel attributes
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	// set cancel function
	// pthread_cleanup_push(video_close, (void *)vc);

	if (videoc->fps) {
		fps = videoc->fps;
		nmsprintf(2, "Video Thread Started: fps = %f\n", fps);
	} else {
		nmsprintf(2, "Video Thread Started: using default fps = %f\n", fps);
	}
	tvsleep.tv_sec = 1/fps;// SLEEP_MS / 1000;
	tvsleep.tv_usec = (long)(1000000/fps) % 1000000; //(SLEEP_MS % 1000) * 1000;
	gettimeofday(&tvstart, NULL);
	tvstop.tv_sec = tvstart.tv_sec;
	tvstop.tv_usec = tvstart.tv_usec;

	for (;;) {
		timeval_add(&tvstart, &tvstart, &tvsleep);
		timeval_subtract(&tvstop, &tvstart, &tvstop);
		select(0, NULL, NULL, NULL, &tvstop);
		funcs->update_screen();
		gettimeofday(&tvstop, NULL);
	}

	// pthread_cleanup_pop(1);

	return NULL;
}

