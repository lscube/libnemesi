/* * 
 *  $Id: audio_driver.h 48 2003-11-10 16:01:50Z mancho $
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

static uint32 preinit(const char *arg);
static uint32 config(int rate,int channels,int format,int flags);
// TODO function "control" for volume
// static int control(int cmd, void *arg);
static uint8 *get_buff(uint32 len);
static uint32 play_buff(uint8 *data, uint32 len);
static void pause(void);
static void resume(void);
static void uninit(void);

#define NMS_LIB_AUDIO(x) NMSAFunctions nms_audio_##x =\
{\
	&info, \
	preinit, \
	config, \
	get_buff, \
	play_buff, \
	pause, \
	resume, \
	uninit \
}

#endif // __AUDIO_DRIVER_H

