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
 *  This file is largely and freely inspired by audio_out_internal.h from MPlayer project.
 *  
 * */

#ifndef __AUDIO_DRIVER_H
#define __AUDIO_DRIVER_H

#include <nemesi/types.h>

// static uint32 preinit(const char *arg);
// static uint32 config(uint32 rate, uint8 channels, uint32 format, uint32 flags);

static uint32 init(uint32 * rate, uint8 * channels, uint32 * format,
		   uint32 buff_ms, uint32 flags, const char *arg);
static uint32 control(uint32 cmd, void *arg);
static uint8 *get_buff(uint32 len);
static uint32 play_buff(uint8 * data, uint32 len, double pts);
static void audio_pause(void);
static void audio_resume(void);
static void reset(void);
static void uninit(void);

#define NMS_LIB_AUDIO(x) nms_au_fnc nms_audio_##x =\
{\
	&info, \
	init, \
	control, \
	get_buff, \
	play_buff, \
	audio_pause, \
	audio_resume, \
	reset, \
	uninit \
}

#endif				// __AUDIO_DRIVER_H
