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

#include <nemesi/decoder.h>
#include <nemesi/output.h>

void dec_clean(void *args)
{
	nms_output *outc = (nms_output *)args;

	// kill Video Thread
	video_th_stop(outc->video);
	// Uninit Output modules
	if ( (outc->video) && (outc->video->init) ) {
		// outc->video->functions->uninit();
		outc->video->functions->reset();
		outc->video->init = 0;
	}
	if ( (outc->audio) && (outc->audio->init) ) {
		outc->audio->functions->pause();
		outc->audio->functions->reset();
	}
	if (outc->diskwriter)
		close_files(outc->diskwriter->fd); 

	nms_printf(NMSML_DBG1, "Decoder Thread R.I.P.\n");
}
