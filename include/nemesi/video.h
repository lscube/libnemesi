/* * 
 *  $Id: video.h 48 2003-11-10 16:01:50Z mancho $
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

#ifndef __VIDEO_H
#define __VIDEO_H

#include <pthread.h>

#include <config.h>

#include <nemesi/types.h>
#include <nemesi/video_img.h>
#include <nemesi/video_drivers.h>

typedef struct {
	// True (1) if initialized
	uint8 init;
	//! thread id
	pthread_t tid;
	// pixel format of video output
	uint32 format;
	// window width
	uint32 width;
	// window height
	uint32 height;
	// frame rate
	float fps;
	//! functions for the specific video output driver
	NMSVFunctions *functions;
	// void *functions;
} NMSVideo;

NMSVideo *video_preinit(char *);
int video_th_start(NMSVideo *);
int video_th_stop(NMSVideo *);
void *video_th(void *);

#endif // __VIDEO_H

