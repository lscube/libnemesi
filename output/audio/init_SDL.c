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
	uiprintf("SAMPLE: requested %hu -> obtained %hu\n\n", requested_fmt.samples, obtained_fmt->samples);
	return obtained_fmt;
}

#endif
