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

#include <nemesi/audio.h>

#ifdef HAVE_SDL

#include <stdlib.h>

SDL_AudioSpec *init_SDL(struct audio_buff *audio_buffer)
{
	SDL_AudioSpec requested_fmt, *obtained_fmt;

	if ( SDL_Init(SDL_INIT_AUDIO | SDL_INIT_NOPARACHUTE) < 0 ) {
		uiprintf("Unable to init SDL: %s\n", SDL_GetError());
		return NULL;
	} else
		uiprintf("\nSDL Initialization successful\n");

	requested_fmt.freq = FREQ;
	requested_fmt.format = AUDIO_S16SYS;
	requested_fmt.channels = CHANNELS;
	requested_fmt.samples = SAMPLES;
	requested_fmt.callback = SDL_mixaudio;
	requested_fmt.userdata = (void *)audio_buffer;

	if( (obtained_fmt = (SDL_AudioSpec *)malloc(sizeof(SDL_AudioSpec))) == NULL ) {
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}

	if ( SDL_OpenAudio(&requested_fmt, obtained_fmt) < 0 ) {
		uiprintf("Unable to open SDL audio: %s\n", SDL_GetError());
		return NULL;
	}
	uiprintf("\nSDL Audio initialization completed successfully\n\n");
	uiprintf("FREQ: requested %d -> obtained %d\n", requested_fmt.freq, obtained_fmt->freq);
	uiprintf("FORMAT: requested %u -> obtained %u\n", requested_fmt.format, obtained_fmt->format);
	uiprintf("CHANNELS: requested %hu -> obtained %hu\n", requested_fmt.channels, obtained_fmt->channels);
	uiprintf("SAMPLE: requested %hu -> obtained %hu\n", requested_fmt.samples, obtained_fmt->samples);
	return obtained_fmt;
}

#endif
