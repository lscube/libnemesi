/* * 
 *  $Id: audio_oss.c 48 2003-11-10 16:01:50Z mancho $
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

#include <nemesi/comm.h>
#include <nemesi/audio_drivers.h>
#include <nemesi/audio_driver.h>

static NMSADrvInfo info = {
	"Open Sound System audio driver",
	"oss",
	"Open Media Streaming Project Team",
	""
};

NMS_LIB_AUDIO(oss);

static uint32 preinit(const char *arg)
{
	return 0;
}

static uint32 config(uint32 rate, uint8 channels, uint32 format, uint32 flags)
{
	return 0;
}

static uint8 *get_buff(uint32 len)
{
	return NULL;
}

static uint32 play_buff(uint8 *data, uint32 len)
{
	return 0;
}

static void pause(void)
{
	return;
}

static void resume(void)
{
	return;
}

static void uninit(void)
{
	return;
}

