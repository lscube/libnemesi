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
 *  This file is largely and freely inspired by audio_out.h from MPlayer project.
 *  
 * */

#ifndef __AUDIO_DRIVERS_H
#define __AUDIO_DRIVERS_H

#include <config.h>
#include <nemesi/types.h>

#define ACTRL_GET_SYSBUF 0
#define ACTRL_GET_ELAPTM 1

typedef struct {
	/* driver name */
	const char *name;
	/* short name (for config strings) (e.g.:"sdl") */
	const char *short_name;
	/* author ("Author name & surname <mail>") */
	const char *author;
	/* any additional comments */
	const char *comment;
} NMSADrvInfo;

typedef struct {
	NMSADrvInfo *info;
	/*
	 * Initialize the audio driver.
	 * params: (in and out)
	 *      rate:
	 *      channels: number of channels
	 *      format:
	 * returns : zero on successful initialization, non-zero on error.
	 */
	// XXX: rate, channels and format are both input and output parameters
	 uint32(*init) (uint32 * rate, uint8 * channels, uint32 * format, uint32 buff_ms, uint32 flags,
			const char *arg);
	/*
	 * Control interface
	 * params:
	 *      cmd: commnand to exec
	 *      arg: argument for command
	 */
	 uint32(*control) (uint32 cmd, void *arg);
	/*
	 * allocs buffer for new decoded data.
	 * params:
	 *      len: length of requested buffer
	 * returns pointer to new buffer
	 */
	uint8 *(*get_buff) (uint32 len);
	/*
	 * This function must be called by decoder after the decoded data has
	 * been written on the allocated buffer
	 * params:
	 *      data: buffer to play.
	 *      len: length of buffer
	 * retuns 1 on error, 0 otherwise
	 */
	 uint32(*play_buff) (uint8 * data, uint32 len, double pts);
	/*
	 * Pauses the driver
	 */
	void (*pause) (void);
	/*
	 * Resume playing
	 */
	void (*resume) (void);
	/*
	 * Reset audio buffer. For seeking pourpose
	 */
	void (*reset) (void);
	/*
	 * Closes driver. Should restore the original state of the system.
	 */
	void (*uninit) (void);
} nms_au_fnc;

#if 0
extern nms_au_fnc nms_audio_oss;
#if HAVE_SDL
extern nms_au_fnc nms_audio_sdl;
#endif
#endif

void list_audio_out(void);
nms_au_fnc *init_best_audio_out(char *, uint32 *, uint8 *, uint32 *, uint32);
/*
char *ao_format_name(int format);
int ao_init(void);

void list_audio_out();

// NULL terminated array of all drivers
extern ao_functions_t* audio_out_drivers[];
 */

#endif				// __AUDIO_DRIVERS_H
