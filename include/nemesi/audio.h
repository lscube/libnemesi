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

#include <stdarg.h>
#include <nemesi/types.h>
#include <nemesi/comm.h>
#include <nemesi/audio_drivers.h>

#include <config.h>

#ifndef GLOBAL_AUDIO_BUFFER
#define AUDIO_EXTERN extern
#else
#define AUDIO_EXTERN
#endif

#ifdef HAVE_SDL

#include <SDL.h>

#define SAMPLES 2048
/* #define SAMPLES 512 */

#endif

#define FREQ 44100
#define CHANNELS 2
#define BYTES_X_SAMPLE 2
#define FORMAT AFMT_S16_LE

typedef struct {
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
	uint8 *audio_data;
/*	uint8 audio_data[AUDIO_BUFF_SIZE]; */
	uint32 read_pos;
	uint32 write_pos;
	uint32 valid_data;
	uint32 len;
#ifndef HAVE_SDL
	int audio_fd;
#endif
	pthread_mutex_t syn;
} NMSAudioBuffer;

AUDIO_EXTERN struct audio_buff *global_audio_buffer;

// struct audio_buff *ab_init(uint32);
NMSAudioBuffer *ab_init(uint32);
uint8 *ab_get(uint32, ...);
/* end of Audio Buffer defines */

NMSAudio *audio_preinit(char *);

/*
int audio_init(void);

#ifdef HAVE_SDL
SDL_AudioSpec *init_SDL(struct audio_buff *);
void SDL_mixaudio(void *, Uint8 *, int);
#else
*/
int set_audiodev(void);
int oss_play(void);
void enable_output(int);
// #endif

int get_sys_buff(void);
float sys_buff_size(float *);

int audio_play();
int audio_pause();

int empty_audio_buffer(void);
int audio_close(void);

#undef GLOBAL_AUDIO_BUFFER
#undef AUDIO_EXTERN

#endif
