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

#include <string.h>
#include <pthread.h>

#include <nemesi/comm.h>
// #include <nemesi/video.h>
#include <nemesi/output.h>

// int video_th_start(NMSVideo *vc)
int video_th_start(NMSOutput *outc)
{
	pthread_attr_t vth_attr;
	int n;

// VIDEO THREAD INITIALIZATION

	/* set Video thread attributes to make thread joinable */
	pthread_attr_init(&vth_attr);

	if ((n = pthread_attr_setdetachstate(&vth_attr, PTHREAD_CREATE_JOINABLE)) != 0) {
		return nmsprintf(NMSML_FATAL, "Cannot set Video Thread attributes!: %s\n", strerror(n));
	}

	/* Create Video Thread */
	if ((n = pthread_create(&(outc->video->tid), &vth_attr, &video_th, (void *)outc)) > 0) {
		return nmsprintf(NMSML_FATAL, "Cannot create Video Thread: %s\n", strerror(n));
	}

	return 0;
}

