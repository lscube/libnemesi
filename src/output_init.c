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

//! with this define we declare global variables contained in video.h
#define NMS_GLOBAL_OUTPUT
#include <nemesi/output.h>

#include <nemesi/preferences.h>
#include <nemesi/comm.h>

int output_init(void)
{
	// Warning: We cannot yet use uiprintf and uierror functions

	if (!(nmsoutc = malloc(sizeof(NMSOutput)))) {
		uierror("Could not alloc output struct");
		return 1;
	}

	uiprintf(SEPARATOR);
	// AUDIO MODULE INIT
	if ((nmsoutc->audio=audio_init("sdl")) == NULL) {
		fprintf(stderr, "Audio module not available\n");
		// fprintf(stderr, "Audio module not available: setting \"output\" to \"disk\"\n");
		// rem_avail_pref("output card");
		// edit_pref("output disk");
	}

	uiprintf(SEPARATOR);
	// VIDEO MODULE INIT
	if ((nmsoutc->video=video_preinit("sdl")) == NULL) {
		if (!nmsoutc->audio) {
			fprintf(stderr, "Video module not available: setting \"output\" to \"null\"\n");
			rem_avail_pref("output card");
			// edit_pref("output disk");
		} else
			fprintf(stderr, "Video module not available\n");
	}

	uiprintf(SEPARATOR);
	// DISKWRITER MODULE INIT
	if ( (nmsoutc->diskwriter=diskwriter_init(DEFAULT_FILENAME)) == NULL ) {
		fprintf(stderr, "Disk Writer module not available\n");
		rem_avail_pref("output disk");
		if ( strcmp("card", get_pref("output")) ) {
			fprintf(stderr, "\nNo output device available\n Cannot continue.\n");
			return 1;
		}
	} else
		uiprintf("Diskwriter succesfully initialized\n");
	uiprintf(SEPARATOR);

	return 0;
}

