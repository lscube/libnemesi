/* * 
 *  $Id: video_init.c 48 2003-11-10 16:01:50Z mancho $
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

#include <stdlib.h>
#include <string.h>
#include <pthread.h>

//! with this define we declare global variables contained in video.h
#define NMS_GLOBAL_VIDEO
#include <nemesi/video.h>

//! with this define we declare global variables contained in video_drivers.h
#define VCT_GLOBAL_VIDEO_DRIVERS
#include <nemesi/video_drivers.h>

#include <config.h>
#include <nemesi/comm.h>

/*!
  Init video
  */
int video_init(NMSVideo **vc, int width, int height, int format)
{
	pthread_mutexattr_t mutex_attr;
	int n;
	pthread_attr_t vth_attr;
	NMSVFunctions *funcs;
	static int initialized=0;

	if ((*vc=malloc(sizeof(NMSVideo))) == NULL)
		return uierror("Could not alloc video structure");

	// Video Output Driver selection
	(*vc)->functions = &nms_video_sdl; // XXX: very very temporanea
	funcs = (*vc)->functions; // XXX: very very temporanea

	/*
	(*vc)->width = width;
	(*vc)->height = height;
	(*vc)->format = format;
	*/

	// mutex initialization
	pthread_mutexattr_init(&mutex_attr); // always returns 0
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if (pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED) > 0)
		return uierror("Error setting mutex attributes for Video Buffer");
#endif

	// video init
	if (!initialized) {
		if (funcs->preinit(NULL))
			return 1;
		initialized = 1;
	}
	// video rectangle init
	/*
	(*vc)->rect_pt = funcs->config(width, height, "ViVoTrA (SDL)", IMGFMT_YV12);
	if (!(*vc)->rect_pt)
		return 1;
	*/

// VIDEO THREAD INITIALIZATION

	/* set Video thread attributes to make thread joinable */
	pthread_attr_init(&vth_attr);
	if (!(*vc)->tid) {
		if ((n = pthread_attr_setdetachstate(&vth_attr, PTHREAD_CREATE_JOINABLE)) != 0) {
			return uierror("Cannot set Video Thread attributes!: %s", strerror(n));
		}

	/* Create Video Thread */
		/* TODO call thread function
		if ((n = pthread_create(&(*vc->tid), &vth_attr, &video_th, (void *) *vc)) > 0) {
			return vcterror("Cannot create Video Thread: %s", strerror(n));
		}
		*/
	}
	return 0;
}

