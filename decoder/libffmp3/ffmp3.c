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

/*#include "mpg123.h"
#include "mpglib.h"*/

/*#include <nemesi/comm.h>*/

/* char buf[16384]; */
/* struct mpstr mp; */

#define BUFFER 8192 

int get_plugin_pt(void);
int decode(char *, int, uint8 *(*)());

int get_plugin_pt(void)
{
	return 96;
}

int decode(char *data, int len, uint8 *(*ab_get)(uint32))
{
	
	/*static struct mpstr *mp = NULL;*/
	static	AVCodec *codec;
    	static AVCodecContext *c= NULL;

	// int size;
	int out_size;//=AVCODEC_MAX_AUDIO_FRAME_SIZE;
	static int16_t out[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	//static uint8_t *out;
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
    
//		out = (uint8_t *)malloc(AVCODEC_MAX_AUDIO_FRAME_SIZE * sizeof(uint8_t));
	}

	/*if(!mp){
		if((mp=(struct mpstr *)malloc(sizeof(struct mpstr))) == NULL){
			fprintf(stderr,"Cannot allocate memory!\n");
			return 1;
		}
		InitMP3(mp);
	}*/

	
	
	while ( len_tmp < (len - 4) ) {
		len_tmp += avcodec_decode_audio(c, (int16_t *)out, (int *)&out_size, (uint8_t *)(data + 4 + len_tmp), (int)(len  - 4 - len_tmp));
		if (out_size > 0){
			audio_data=(*ab_get)((uint32)out_size);
			memcpy(audio_data, out, out_size);
		}
	}

	/*ret = decodeMP3(mp, data+4, len-4, out, BUFFER, &size);*/
	/*while(ret == MP3_OK) {
	--canc-	write(audio_fd, out, size); 
		audio_data=(*ab_get)((uint32)size);
		memcpy(audio_data, out, size);
		ret = decodeMP3(mp, NULL, 0, out, BUFFER, &size);
	}*/

	return 0;
}
