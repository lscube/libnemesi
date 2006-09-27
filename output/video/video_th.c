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

#define AV_SYNC
// #undef AV_SYNC

#include <sys/select.h>
#include <sys/time.h>

// #include <nemesi/video.h>
#include <nemesi/output.h>
#include <nemesi/comm.h>
#include <nemesi/utils.h>
#include <nemesi/types.h>

#define SLEEP_MS 40
#define DEF_FPS 25.0		// must be float
// #define MAX_AV_THRES 100

void *video_th(void *outc)
{
	nms_video *videoc = ((nms_output *) outc)->video;
	nms_audio *audioc = ((nms_output *) outc)->audio;
	nms_vid_fnc *vfuncs = videoc->functions;
	nms_au_fnc *afuncs = NULL;
	struct timeval tvsleep = { 0, 0 }, tvstart, tvstop;
	float fps = DEF_FPS;
	double last_pts = 0, next_pts = 0;
#ifdef AV_SYNC
	double audio_elapsed = 0;
#endif				// AV_SYNC

	// pthread cancel attributes
	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
	// set cancel function
	// pthread_cleanup_push(video_close, (void *)vc);

	if (audioc)
		afuncs = audioc->functions;

	if (videoc->fps) {
		fps = videoc->fps;
		nms_printf(NMSML_VERB, "Video Thread Started: fps = %f\n", fps);
	} else {
		nms_printf(NMSML_VERB,
			   "Video Thread Started: using default fps = %f\n",
			   fps);
	}

//      tvsleep.tv_sec = 1/fps;// SLEEP_MS / 1000;
//      tvsleep.tv_usec = (long)(1000000/fps) % 1000000; //(SLEEP_MS % 1000) * 1000;

	gettimeofday(&tvstart, NULL);
	tvstop.tv_sec = tvstart.tv_sec;
	tvstop.tv_usec = tvstart.tv_usec;

	for (;;) {
		timeval_add(&tvstart, &tvstart, &tvsleep);
		if (!timeval_subtract(&tvstop, &tvstart, &tvstop)
		    && ((tvstop.tv_sec > 0) || (tvstop.tv_usec > 10000))) {
			nms_printf(NMSML_DBG3, "sleep for: %ld %ld\n",
				   tvstop.tv_sec, tvstop.tv_usec);
			select(0, NULL, NULL, NULL, &tvstop);
		} else
			nms_printf(NMSML_DBG3, "didn't sleep\n");
		vfuncs->update_screen(&next_pts);
		nms_printf(NMSML_DBG2,
			   "next presentation timestamp is: %3.2f\n", next_pts);
		// gettimeofday(&tvstop, NULL);
		if (!next_pts)
			next_pts = last_pts + 1000 / fps;
#ifdef AV_SYNC
		if (audioc && audioc->init)
			afuncs->control(ACTRL_GET_ELAPTM, &audio_elapsed);
		if (audio_elapsed) {
			nms_statusprintf(ELAPSED_STATUS,
					 "Elapsed: V: %3.2lfms\tA: %3.2lfms\tsync A-V: %3.2lfms",
					 last_pts, audio_elapsed,
					 next_pts - audio_elapsed);
			if (next_pts < audio_elapsed)
				tvsleep.tv_sec = tvsleep.tv_usec = 0;
			else {	/*if ( next_pts - audio_elapsed > MAX_AV_THRES ) {
				   tvsleep.tv_sec = 0;
				   tvsleep.tv_usec = ( next_pts + MAX_AV_THRES ) * 1000;
				   } else */
				tvsleep.tv_sec =
				    (next_pts - audio_elapsed) / 1000;
				tvsleep.tv_usec =
				    (next_pts - audio_elapsed -
				     tvsleep.tv_sec * 1000) * 1000;
			}
		} else {
#endif				// AV_SYNC
			nms_statusprintf(ELAPSED_STATUS, "Elapsed: V: %3.2lfms",
					 last_pts);
			tvsleep.tv_sec = (next_pts - last_pts) / 1000;
			tvsleep.tv_usec =
			    (next_pts - last_pts -
			     tvsleep.tv_sec * 1000) * 1000;
		}
		/*
		   else {
		   tvsleep.tv_sec = 1/fps;
		   tvsleep.tv_usec = (long)(1000000/fps) % 1000000;
		   nms_printf(NMSML_DBG2, "\n\tnextp_pts = 0\n");
		   }
		 */
		last_pts = next_pts;
		gettimeofday(&tvstop, NULL);
	}

	// pthread_cleanup_pop(1);

	return NULL;
}
