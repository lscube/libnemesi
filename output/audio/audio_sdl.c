/* * 
 *  $Id: audio_sdl.c 48 2003-11-10 16:01:50Z mancho $
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
#include <SDL.h>

#include <nemesi/comm.h>
#include <nemesi/utils.h>
#include <nemesi/audio_drivers.h>
#include <nemesi/audio_driver.h>
#include <nemesi/audio_format.h>
#include <nemesi/audio.h>

//! define this if you want to choose the audio subdevice for SDL lib. According to SDL documentation: "Using these variables isn't recommened and the names and presence of these variables aren't guaranteed from one release to the next". However... they are useful.
#define SDLENV 1

static NMSADrvInfo info = {
	"SDL Library audio driver",
	"sdl",
	"Open Media Streaming Project Team",
	""
};

NMS_LIB_AUDIO(sdl);

#define SAMPLES 2048

static struct sdl_priv_s {
	SDL_AudioSpec aspec;
	NMSAudioBuffer *audio_buffer;
} sdl_priv;

#ifdef SDLENV
#ifndef HAVE_SETENV
static void setenv(const char *name, const char *val, int _xx)
{
  int len  = strlen(name) + strlen(val) + 2;
  char *env = malloc(len);

  if (env != NULL) {
    strcpy(env, name);
    strcat(env, "=");
    strcat(env, val);
    putenv(env);
  }
}
#endif // SETENV
#endif // SDLENV

static void SDL_mixaudio(void *userdata, Uint8* stream, int len)
{
	struct audio_buff *audio_buffer = (struct audio_buff *)userdata;
	uint32 bytes_to_copy;
	uint32 to_valid=0;
	uint32 prev_to_valid;

	pthread_mutex_lock(&(audio_buffer->syn));
	bytes_to_copy=min((uint32)len,audio_buffer->len);
	while( bytes_to_copy > 0) {
		prev_to_valid = to_valid;
		to_valid= min(bytes_to_copy,(audio_buffer->valid_data-audio_buffer->read_pos));
		memcpy(stream + prev_to_valid, (audio_buffer->audio_data + audio_buffer->read_pos), to_valid);
		/*
		SDL_MixAudio(stream + prev_to_valid, (audio_buffer->audio_data + audio_buffer->read_pos), to_valid, SDL_MIX_MAXVOLUME);
		*/
		if ( (audio_buffer->read_pos + to_valid) < audio_buffer->valid_data )
			audio_buffer->read_pos += to_valid;
		else {
			audio_buffer->read_pos=0;
			audio_buffer->valid_data=audio_buffer->write_pos;
		}
		bytes_to_copy -= to_valid;
		audio_buffer->len -= to_valid;
	}
	pthread_mutex_unlock(&(audio_buffer->syn));

	return;
}

static uint32 preinit(const char *arg)
{
	Uint32 subsystem_init;
	Uint32 flags=0;

	subsystem_init = SDL_WasInit(SDL_INIT_EVERYTHING);

	if (!(subsystem_init & SDL_INIT_AUDIO))
		flags |= SDL_INIT_AUDIO;

#ifdef SDLENV
	if (arg) {
		setenv("SDL_AUDIODRIVER", arg, 1);
		uiprintf("SDL: using %s audio driver\n", arg);
	}
#endif // SDLENV

	if (!flags) {
		uiprintf("SDL Audio already initialized\n");
	} else {
		uiprintf("Initializing SDL Audio output\n");
		if (subsystem_init) {
			if (SDL_InitSubSystem(flags))
				return uierror("Could not initialize SDL Audio");
		} else {
			flags |= SDL_INIT_NOPARACHUTE;
			if (SDL_Init(flags))
				return uierror("Could not initialize SDL Audio");
		}
		uiprintf("SDL Audio initialized\n");
	}

	return 0;
}

static uint32 config(uint32 rate, uint8 channels, uint32 format, uint32 flags)
{
	SDL_AudioSpec requested_fmt;

	if ( (sdl_priv.audio_buffer=ab_init(AUDIO_BUFF_SIZE)) == NULL )
		return uierror("Failed while initializing Audio Buffer\n");

	switch(format) {
	    case AFMT_U8:
		requested_fmt.format = AUDIO_U8;
	    break;
	    case AFMT_S16_LE:
		requested_fmt.format = AUDIO_S16LSB;
	    break;
	    case AFMT_S16_BE:
		requested_fmt.format = AUDIO_S16MSB;
	    break;
	    case AFMT_S8:
		requested_fmt.format = AUDIO_S8;
	    break;
	    case AFMT_U16_LE:
		requested_fmt.format = AUDIO_U16LSB;
	    break;
	    case AFMT_U16_BE:
		requested_fmt.format = AUDIO_U16MSB;
	    break;
	    default:
                return uierror("SDL: Unsupported audio format: %s (0x%x).\n", audio_format_name(format), format);
		break;
	}

	requested_fmt.freq = rate;
	requested_fmt.channels = channels;
	requested_fmt.samples = SAMPLES;
	requested_fmt.callback = SDL_mixaudio;
	requested_fmt.userdata = (void *)(sdl_priv.audio_buffer);

	if ( SDL_OpenAudio(&requested_fmt, &(sdl_priv.aspec)) < 0 )
		return uierror("SDL: unable to open audio: %s\n", SDL_GetError());

	uiprintf("\nSDL Audio initialization completed successfully\n\n");
	uiprintf("FREQ: requested %d -> obtained %d\n", requested_fmt.freq, sdl_priv.aspec.freq);
	uiprintf("FORMAT: requested %u -> obtained %u\n", requested_fmt.format, sdl_priv.aspec.format);
	uiprintf("CHANNELS: requested %hu -> obtained %hu\n", requested_fmt.channels, sdl_priv.aspec.channels);
	uiprintf("SAMPLE: requested %hu -> obtained %hu\n", requested_fmt.samples, sdl_priv.aspec.samples);

	return 0;
}

static uint8 *get_buff(uint32 len)
{

	return ab_get(len);
}

static uint32 play_buff(uint8 *data, uint32 len)
{
	return 0;
}

static void pause(void)
{
	SDL_PauseAudio(1);

	return;
}

static void resume(void)
{
	SDL_PauseAudio(0);

	return;
}

static void uninit(void)
{
	SDL_CloseAudio();
	SDL_QuitSubSystem(SDL_INIT_AUDIO);

	return;
}

