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

#include <nemesi/comm.h>
#include <nemesi/audio.h>
#include <nemesi/audio_drivers.h>

NMSAFunctions *init_best_audio_out(const char *drv)
{
	int i=0;
	// XXX: vettore temporaneamente dichiarato qui
	NMSAFunctions* audio_out_drivers[]= {
		&nms_audio_oss,
#if HAVE_SDL
		&nms_audio_sdl,
#endif
		NULL
	};

	while (audio_out_drivers[i] && strcmp(drv, audio_out_drivers[i]->info->short_name))
		i++;
	if (!audio_out_drivers[i]) {
		nmserror("Could not find audio driver %s", drv);
		return NULL;
	}

	nmsprintf(3, "Found driver %s (i=%d)\n", audio_out_drivers[i]->info->name, i);
	if (audio_out_drivers[i]->init(FREQ, CHANNELS, FORMAT, 0, NULL)) {
		nmserror("Could not initialize driver %s", drv);
		return NULL;
	}

	return audio_out_drivers[i];
}
