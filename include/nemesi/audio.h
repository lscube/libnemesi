/* * 
 *  ./include/nemesi/audio.h: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
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

#include <config.h>

#ifdef HAVE_SDL

#include <SDL.h>

#define SAMPLES 2048

#endif

#define FREQ 44100
#define CHANNELS 2
#define BYTES_X_SAMPLE 2

/* Audio Buffer defines */
#define SECONDS 1
#define AUDIO_BUFF_SIZE (unsigned long)(FREQ*SECONDS*CHANNELS*BYTES_X_SAMPLE)

struct audio_buff {
/*	uint8 *audio_data; */
	uint8 audio_data[AUDIO_BUFF_SIZE];
	uint32 read_pos;
	uint32 write_pos;
	uint32 valid_data;
	uint32 len;
#ifndef HAVE_SDL
	int audio_fd;
#endif
	pthread_mutex_t syn;
};

struct audio_buff *global_audio_buffer;

struct audio_buff *ab_init(void);
uint8 *ab_get(uint32 len, ...);
/* end of Audio Buffer defines */

int audio_init(void);

#ifdef HAVE_SDL
SDL_AudioSpec *init_SDL(struct audio_buff *);
void SDL_mixaudio(void *, Uint8 *, int);
#else
int set_audiodev(void);
int oss_play(void);
void enable_output(int);
#endif

int get_sys_buff(void);
float sys_buff_size(float *);

int audio_play();
int audio_pause();

int empty_audio_buffer(void);
int audio_close(void);

#endif
