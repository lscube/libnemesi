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

#include <stdlib.h>
#include <string.h>

#include <nemesi/audio.h>

#include <nemesi/audio_drivers.h>

#include <config.h>
#include <nemesi/comm.h>

/*!
  Init audio
  */
NMSAudio *audio_init(char *drv_hint)
{
	NMSAudio *ac;
	// NMSAFunctions *funcs;

	if ((ac=malloc(sizeof(NMSAudio))) == NULL) {
		nmserror("Could not alloc audio structure");
		return NULL;
	}

	ac->init = 0;

	// Audio Output Driver selection
	if ( !(ac->functions = init_best_audio_out(drv_hint)) )
		return NULL;
	else
		ac->init = 1;
	
	nmsprintf(1, "Audio driver: %s\n", ac->functions->info->name);

	return ac;
}

