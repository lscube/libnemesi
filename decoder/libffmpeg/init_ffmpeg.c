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

#include "ffmpeg.h"

FFMpegDec *init_ffmpeg(void)
{
	FFMpegDec *ff;

	if (!(ff=malloc(sizeof(FFMpegDec)))) {
		fprintf(stderr, "Could not alloc memory for FFMpeg data structures\n");
		return NULL;
	}
	/* must be called before using avcodec lib */
	avcodec_init();

	//register_avcodec(&mp3_decoder);
	avcodec_register_all();

	/* find the mpeg audio decoder */
	if (!(ff->codec = avcodec_find_decoder(CODEC_ID_MPEG1VIDEO))) {
		fprintf(stderr, "libffmpeg: codec MPEG1VIDEO not found\n");
		return NULL;
	}
	ff->context = avcodec_alloc_context();
	ff->frame = avcodec_alloc_frame();
	
	if(ff->codec->capabilities&CODEC_CAP_TRUNCATED)
		ff->context->flags|= CODEC_FLAG_TRUNCATED; /* we dont send complete frames */
 
	/* open it */
	if (avcodec_open(ff->context, ff->codec) < 0) {
		fprintf(stderr, "libffmpeg: could not open codec\n");
		return NULL;
	}

	ff->got_frame = 0;
	
	fprintf(stderr, "\nlibavcodec decoder plugin succesfully initialized\n");
   
	return ff;
}
