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

// #include <unistd.h>
// #include <string.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <math.h>
#include "ffmpeg.h"

#include <nemesi/output.h>
#include <nemesi/video_img.h>

// #ifdef HAVE_AV_CONFIG_H
// #undef HAVE_AV_CONFIG_H
// #endif

int get_plugin_pt(void);
int decode(char *, int, NMSOutput *);


int get_plugin_pt(void)
{
	return 32;
}


int decode(char *data, int len, NMSOutput *outc)
{
	NMSVideo *vc = outc->video;
	NMSVFunctions *funcs = vc->functions;
	NMSPicture pict;
	NMSPicture *pict_pt = &pict;
	int len_tmp=0, size;
	static FFMpegDec *ff=NULL;

	//return 0;
	if ( (!ff) && (!(ff=init_ffmpeg())) )
		return 1;

// XXX: provvisoriamente commentata
#if 0
	if (!data) {
			/* some codecs, such as MPEG, transmit the I and P frame with a
		 	* latency of one frame. You must do the following to have a
		 	* chance to get the last frame of the video */
			
			//� l'ultima chiamata
			
			len_tmp += avcodec_decode_video(c, picture, &got_picture, NULL, 0);
    		
			if (got_picture) {
			
				video_data=(*(outc->video.vb_get))((uint8)1/*got_picture*/);
				
				memcpy(video_data.a, picture->data[0], /*got_picture*/(outc->video.dimframe));
				memcpy(video_data.b, picture->data[1], /*got_picture*/(outc->video.dimframe)/4);
				memcpy(video_data.c, picture->data[2], /*got_picture*/(outc->video.dimframe)/4);
		
			}
			return 0;
		}
#endif // if 0
	while ( len_tmp < (len - 4) ) {
		
		/*by sbiro: "picture" � ptr al frame decodificato, in "got_picture" viene restituito il num di picture
	     	*          "inbuf_ptr" � il ptr all'area dati di ingresso, "size" � la dimensione dei dati in ingresso
	     	*          "len" � il num di byte letti e decodificati*/
	    
		
		size= avcodec_decode_video(ff->context, ff->frame, &(ff->got_frame), (uint8_t *)(data + 4 /*+ len_tmp*/), (int)(len - 4/* - len_tmp*/));
                
		
		
		if (size < 0) {
                	fprintf(stderr, "Error while decoding with libavcodec\n");
                	return 1;
            	} else if (ff->got_frame){
			if (!vc->tid) {
#if 0
				outc->video.w = c->width;
				//fprintf(stderr, "DECODE: width = %d\n",outc->video.w);
				outc->video.h = c->height;
				//fprintf(stderr, "DECODE: height = %d\n",outc->video.h);
				outc->video.format = IMGFMT_I420;
				outc->video.linesize = picture->linesize[0];
				//fprintf(stderr, "DECODE: linesize = %d\n",outc->video.linesize);
				outc->video.planes = 3;
				outc->video.dimframe = (uint32)(picture->linesize[0])*(c->height);
				//fprintf(stderr, "DECODE: dimframe = %d\n",outc->video.dimframe);
#endif // if 0
				vc->format = IMGFMT_YV12;
				vc->width = ff->context->width;
				vc->height = ff->context->height;
				if (funcs->config(vc->width, vc->height, vc->width, vc->height, \
							0, "NeMeSI (SDL)", IMGFMT_YV12))
					return 1;
				vc->init = 1;
			}
			funcs->get_picture(ff->context->width, ff->context->height, pict_pt);
			img_convert((AVPicture *)pict_pt, vc->format, (AVPicture *)ff->frame, ff->context->pix_fmt, \
					ff->context->width, ff->context->height);
			funcs->draw_picture(pict_pt);
			fprintf(stderr, "\n\tdraw_pict\n");
			// video_data=(*(outc->video.vb_get))((uint8)1/*got_picture*/);
				
			// memcpy(video_data.a, picture->data[0], /*got_picture*/(outc->video.dimframe));
			// memcpy(video_data.b, picture->data[1], /*got_picture*/(outc->video.dimframe)/4);
			// memcpy(video_data.c, picture->data[2], /*got_picture*/(outc->video.dimframe)/4);
                	
			
			//got_picture--;
		}
		len_tmp += size;
	}	
		
		
		

	return 0;
}
