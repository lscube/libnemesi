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

#include <nemesi/video.h>

#include <nemesi/video_drivers.h>

#include <config.h>
#include <nemesi/comm.h>

/*!
  Init video
  */
NMSVideo *video_preinit(char *drv_hint)
{
	NMSVideo *vc;
	NMSVFunctions *funcs;

	if ((vc=malloc(sizeof(NMSVideo))) == NULL) {
		uierror("Could not alloc video structure");
		return NULL;
	}

	// Video Output Driver selection
	vc->functions = &nms_video_sdl; // XXX: very very temporanea
	funcs = vc->functions;

	// TODO: parse drv_hint for subdriver

	// video init
	if (funcs->preinit(NULL)) // TODO: send subdriver hint
		return NULL;
	
	uiprintf("Video driver: %s\n", funcs->info->name);

	return vc;
}

