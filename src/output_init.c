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

//! with this define we declare global variables contained in video.h
#define NMS_GLOBAL_OUTPUT
#include <nemesi/output.h>

#include <nemesi/main.h>
#include <nemesi/preferences.h>
#include <nemesi/comm.h>

int output_init(nms_out_hints * hints)
{
	if (!(nms_outc = malloc(sizeof(nms_output))))
		return nms_printf(NMSML_FATAL,
				  "Could not alloc output struct");

	nms_outc->elapsed = 0;
	nms_outc->sysbuff_ms = (hints
				&& hints->sysbuff_ms) ? hints->
	    sysbuff_ms : DEF_SYSBUFF_MS;

	nms_printf(NMSML_NORM, SEPARATOR);
	// AUDIO MODULE INIT
	// if ((nms_outc->audio=audio_init("sdl")) == NULL) {
	if (hints && hints->audio && !strcmp(hints->audio, "noaudio")) {
		nms_printf(NMSML_NORM, "No Audio\n");
		nms_outc->audio = NULL;
	} else
	    if ((nms_outc->audio =
		 audio_init(hints ? hints->audio : NULL,
			    nms_outc->sysbuff_ms)) == NULL) {
		nms_printf(NMSML_ERR, "Audio module not available\n");
		// fprintf(stderr, "Audio module not available: setting \"output\" to \"disk\"\n");
		// rem_avail_pref("output card");
		// edit_pref("output disk");
	}

	nms_printf(NMSML_NORM, SEPARATOR);
	// VIDEO MODULE INIT
	if (hints && hints->video && !strcmp(hints->video, "novideo")) {
		nms_printf(NMSML_NORM, "No Video\n");
		nms_outc->video = NULL;
	} else
	    if ((nms_outc->video =
		 video_preinit(hints ? hints->video : NULL,
			       nms_outc->sysbuff_ms * 2 /*XXX: tmp */ )) ==
		NULL) {
		if (!nms_outc->audio) {
			nms_printf(NMSML_ERR,
				   "Video module not available: setting \"output\" to \"null\"\n");
			rem_avail_pref("output card");
			// edit_pref("output disk");
		} else
			nms_printf(NMSML_ERR, "Video module not available");
	}

	nms_printf(NMSML_NORM, SEPARATOR);
	// DISKWRITER MODULE INIT
	if (hints && hints->diskwriter && !strcmp(hints->diskwriter, "nodisk")) {
		nms_printf(NMSML_NORM, "No DiskWriter\n");
		nms_outc->diskwriter = NULL;
	} else
	    if ((nms_outc->diskwriter =
		 diskwriter_init(hints ? hints->diskwriter : NULL)) == NULL) {
		nms_printf(NMSML_ERR, "Disk Writer module not available\n");
		rem_avail_pref("output disk");
		if (strcmp("card", get_pref("output")))
			return nms_printf(NMSML_FATAL,
					  "No output device available\n Cannot continue\n");
	} else
		nms_printf(NMSML_NORM, "Diskwriter succesfully initialized\n");
	nms_printf(NMSML_NORM, SEPARATOR);

	return 0;
}
