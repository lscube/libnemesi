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

#ifndef __NEMESI_OUTPUT_H
#define __NEMESI_OUTPUT_H

#include <nemesi/video.h>
#include <nemesi/audio.h>
#include <nemesi/diskwriter.h>
#include <nemesi/types.h>

#ifndef NMS_GLOBAL_OUTPUT // declared just one time
#define OUTPUT_EXTERN extern
#else
#define OUTPUT_EXTERN
#endif

#define DEF_SYSBUFF_MS 500

typedef struct {
	double elapsed;
	uint32 sysbuff_ms;
	NMSAudio *audio;
	NMSVideo *video;
	NMSDiskWriter *diskwriter;
} NMSOutput;

OUTPUT_EXTERN NMSOutput *nmsoutc;

int video_th_start(NMSOutput *);
int video_th_stop(NMSVideo *);
void *video_th(void *);

#undef NMS_GLOBAL_OUTPUT
#undef OUTPUT_EXTERN

#endif // __NEMESI_OUTPUT_H

