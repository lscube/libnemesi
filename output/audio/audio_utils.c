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

#include <nemesi/audio.h>
// #include <nemesi/audio_drivers.h>
#include <nemesi/comm.h>

extern NMSAFunctions nms_audio_oss;
#ifdef HAVE_SDL
extern NMSAFunctions nms_audio_sdl;
#endif

static NMSAFunctions *audio_out_drivers[] = {
#if HAVE_SDL
	&nms_audio_sdl,
#endif
	&nms_audio_oss,
	NULL // end of list
};

void list_audio_out(void)
{
	int i;

	nmsprintf(1, "Avalilable audio out drivers are:\n");
	for (i=0; audio_out_drivers[i]; i++) {
		nmsprintf(0, "\t%s\t%s\n", audio_out_drivers[i]->info->short_name, \
				audio_out_drivers[i]->info->name);
	}
	nmsprintf(1, "\n");
}

NMSAFunctions *init_best_audio_out(char *drv, uint32 *rate, uint8 *channels, uint32 *format, uint32 sysbuff_ms)
{
	int i;
	char *device, *sub_device = NULL;

	if ( drv && *drv ) {
		device = strtok(drv, ",");
		while ( device && *device && strcmp(device, "...") ) {
			for (i=0;audio_out_drivers[i] && strcmp(device, audio_out_drivers[i]->info->short_name);i++);
			if (!audio_out_drivers[i])
				nmserror("Could not find audio driver %s", device);
			else {
				nmsprintf(3, "Found audio output driver %s\n", audio_out_drivers[i]->info->name);
				if (!audio_out_drivers[i]->init(rate, channels, format, sysbuff_ms, 0, NULL)) {
					return audio_out_drivers[i];
				}
			}
			device = strtok(NULL, ",");
		}
		if (!device)
			return NULL;
		nmsprintf(2, "Falling back to drivers not hinted\n");
	}
	for (i=0; audio_out_drivers[i]; i++) {
		if (!audio_out_drivers[i]->init(rate, channels, format, sysbuff_ms, 0, NULL))
			return audio_out_drivers[i];
	}

	nmserror("Could not find any working audio driver");

	return NULL;
}

