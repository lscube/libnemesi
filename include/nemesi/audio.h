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

#ifndef __AUDIO_H
#define __AUDIO_H

#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

// #include <stdarg.h>
#include <nemesi/types.h>
#include <nemesi/comm.h>
#include <nemesi/audio_format.h>
#include <nemesi/audio_drivers.h>

#include <config.h>

#define FREQ 44100
#define CHANNELS 2
#define BYTES_X_SAMPLE 2
#define FORMAT AFMT_S16_LE

typedef struct {
	// True (1) if initialized
	uint8 init;
	//! Functions for the specific audio output driver
	NMSAFunctions *functions;
} NMSAudio;

/* Audio Buffer defines */
#define SECONDS 1
#define AUDIO_BUFF_SIZE (unsigned long)(FREQ*SECONDS*CHANNELS*BYTES_X_SAMPLE)
#define AUDIO_SYS_BUFF 0.5 /* Buffer first fill percentage */
#define MIN_AUDIO_SYS_BUFF 0.2 /* min buffer percentage to request fast cycles */
#define MAX_AUDIO_SYS_BUFF 0.9 /* max buffer percentage */

typedef struct audio_buff {
	uint8 *audio_data; //!< audio buffer data
/*	uint8 audio_data[AUDIO_BUFF_SIZE]; */
	uint32 buff_size; //!< total size of allocated buffer
	uint32 read_pos; //!< read position in ring buffer
	uint32 write_pos; //!< write position in ring buffer
	uint32 valid_data; //!< position of buffer fill level
	uint32 len; //!< len of data in buffer
	pthread_mutex_t syn;
	pthread_cond_t cond_full;
} NMSAudioBuffer;

NMSAudioBuffer *ab_init(uint32);
uint8 *ab_get(uint32, ...);
void ab_uninit(NMSAudioBuffer *);
/* end of Audio Buffer defines */

NMSAudio *audio_init(const char *);

#endif
