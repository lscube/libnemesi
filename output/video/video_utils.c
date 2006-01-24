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

#include <nemesi/video_drivers.h>
#include <nemesi/comm.h>
#include <nemesi/utils.h>

#if HAVE_SDL
extern nms_vid_fnc nms_video_sdl;
#endif

static nms_vid_fnc *video_out_drivers[] = {
#if HAVE_SDL
	&nms_video_sdl,
#endif
	NULL
};

void list_video_out(void)
{
	int i;

	nms_printf(NMSML_NORM, "Available video out drivers are:\n");
	for (i=0; video_out_drivers[i]; i++) {
		nms_printf(NMSML_ALWAYS, "\t%s\t%s\n", video_out_drivers[i]->info->short_name, \
			video_out_drivers[i]->info->name);
	}
	nms_printf(NMSML_ALWAYS, "\n");
}

/*!
 * params drv coma separated list of device drivers in the form <driver:option>
 */
nms_vid_fnc *init_best_video_out(char *drv, uint32 sysbuff_ms)
{
	int i;
	char *device, *sub_device = NULL;

	if ( drv && *drv ) {
		device = strtok(drv, ",");
		while ( device && *device && strcmp(device, "...") ) {
			for (i=0;video_out_drivers[i] && strcmp(device, video_out_drivers[i]->info->short_name);i++);
			if (!video_out_drivers[i])
				nms_printf(NMSML_ERR, "Could not find video driver %s", device);
			else {
				nms_printf(NMSML_VERB, "Found video output driver %s\n", video_out_drivers[i]->info->name);
				if (!video_out_drivers[i]->preinit(sub_device, sysbuff_ms)) {
					return video_out_drivers[i];
				}
			}
			device = strtok(NULL, ",");
		}
		if (!device)
			return NULL;
		nms_printf(NMSML_NORM, "Falling Back to drivers not hinted\n");
	}
	for (i=0; video_out_drivers[i]; i++) {
		if (!video_out_drivers[i]->preinit(NULL, sysbuff_ms))
			return video_out_drivers[i];
	}

	nms_printf(NMSML_ERR, "Could not find any working video driver\n");

	return NULL;
}

