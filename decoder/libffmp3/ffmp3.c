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

#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <nemesi/types.h>

#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif

#include <ffmpeg/avcodec.h>

#define BUFFER 8192 

int get_plugin_pt(void);
int decode(char *, int, uint8 *(*)());

int get_plugin_pt(void)
{
	return 96;
}

int decode(char *data, int len, uint8 *(*ab_get)(uint32))
{
	
	static	AVCodec *codec;
    	static AVCodecContext *c= NULL;
	int out_size;
	static int16_t out[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	int len_tmp=0;
	uint8 *audio_data;

	if (!c){

		register_avcodec(&mp3_decoder);
		//avcodec_register_all();
	
		/* find the mpeg audio decoder */
		codec = avcodec_find_decoder(CODEC_ID_MP3LAME);
		if (!codec) {
			fprintf(stderr, "codec not found\n");
			exit(1);
		}
		c = avcodec_alloc_context();
	
		/* open it */
		if (avcodec_open(c, codec) < 0) {
			fprintf(stderr, "could not open codec\n");
			exit(1);
		}
    
	}

	while ( len_tmp < (len - 4) ) {
		len_tmp += avcodec_decode_audio(c, (int16_t *)out, (int *)&out_size, (uint8_t *)(data + 4 + len_tmp), (int)(len  - 4 - len_tmp));
		if (out_size > 0){
			audio_data=(*ab_get)((uint32)out_size);
			memcpy(audio_data, out, out_size);
		}
	}

	return 0;
}
