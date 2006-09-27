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
#include <nemesi/output.h>

#include <nemesi/plugin.h>

plugin_init("MPA", 14)
#ifdef HAVE_AV_CONFIG_H
#undef HAVE_AV_CONFIG_H
#endif
#include <ffmpeg/avcodec.h>
#define BUFFER 8192
#define RESAMPLED
int get_plugin_pt(void);

int get_plugin_pt(void)
{
	return 14;
}

static int decode(char *data, int len, nms_output * outc)
{

	nms_au_fnc *funcs = NULL;	// = outc->audio->functions;
	static AVCodec *codec;
	static AVCodecContext *c = NULL;
	int out_size;
	static int16_t out[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	int len_tmp = 0;
	uint8 *audio_data;
#ifdef RESAMPLED
	// audio resample
	static ReSampleContext *resample_c = NULL;
	uint8 channels;
	uint32 rate;
	int16_t out_resampled[AVCODEC_MAX_AUDIO_FRAME_SIZE];
	int in_samples;
#endif				// RESAMPLED
	int16_t *dec_data = out;

	if (outc->audio)
		funcs = outc->audio->functions;
	else
		return 1;

	if (!c) {

		// register_avcodec(&mp3_decoder);
		avcodec_register_all();

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
		// fprintf(stderr, "channels: %d, rate: %d\n", outc->audio->channels, outc->audio->rate);

	}
#ifdef RESAMPLED
	channels = outc->audio->channels;
	rate = outc->audio->rate;
#endif				// RESAMPLED

	while (len_tmp < (len)) {
		len_tmp +=
		    avcodec_decode_audio(c, out, &out_size,
					 (uint8_t *) (data + len_tmp),
					 len - len_tmp);
		if (out_size > 0) {
#ifdef RESAMPLED
			// fprintf(stderr, "%d,%d\n", c->sample_rate, c->channels);
			// fprintf(stderr, "out size:%d", out_size);
			if ((rate != (uint32) c->sample_rate)
			    || (channels != (uint8) c->channels)) {
				if (!resample_c) {
					fprintf(stderr,
						"initilizing resampler... ");
					resample_c =
					    audio_resample_init(channels,
								c->channels,
								rate,
								c->sample_rate);
					if (!resample_c) {
						fprintf(stderr, "failed!!!\n");
						return 1;
					}
					fprintf(stderr, "done\n");
				}
				in_samples = out_size / (c->channels * 2);	// *2 bacause of 16bit samples.
				// fprintf(stderr, " - in_samples:%d", in_samples);
				audio_resample(resample_c, out_resampled, out,
					       in_samples);
				out_size *= channels / c->channels * 2;
				// fprintf(stderr, " - out_size:%d\n", out_size);
				dec_data = out_resampled;
			}
#endif				// RESAMPLED
			// fprintf(stderr, "\n");
			audio_data = funcs->get_buff((uint32) out_size);
			memcpy(audio_data, dec_data, out_size);
			funcs->play_buff(audio_data, (uint32) out_size,
					 outc->elapsed);
		}
	}

	return 0;
}

const char *get_plugin_encname(void)
{
	return "MPA";
}
