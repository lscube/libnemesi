#include <nemesi/audio.h>

#ifdef HAVE_SDL

#include <nemesi/utils.h>

#include <unistd.h>
#include <string.h>

void SDL_mixaudio(void *userdata, Uint8* stream, int len)
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
#endif
