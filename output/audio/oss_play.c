#include <nemesi/audio.h>

#ifndef HAVE_SDL

#include <sys/ioctl.h>
#include <sys/soundcard.h>

#include <nemesi/utils.h>

int oss_play(void)
{
	struct audio_buff *audio_buffer = global_audio_buffer;
	audio_buf_info info;
	uint32 bytes_to_copy, len_in_frags, to_valid, prev_to_valid;

	pthread_mutex_lock(&(audio_buffer->syn));
	ioctl(audio_buffer->audio_fd,SNDCTL_DSP_GETOSPACE, &info);
	len_in_frags = audio_buffer->len/info.fragsize;
	bytes_to_copy = (min(len_in_frags, info.fragments))*info.fragsize;
	while (bytes_to_copy > 0) {
		prev_to_valid = to_valid;
		to_valid = min(bytes_to_copy,(audio_buffer->valid_data - audio_buffer->read_pos));
		write(audio_buffer->audio_fd, (audio_buffer->audio_data + audio_buffer->read_pos), to_valid);
		if ( (audio_buffer->read_pos + to_valid) < audio_buffer->valid_data )
			audio_buffer->read_pos += to_valid;
		else {
			audio_buffer->read_pos = 0;
			audio_buffer->valid_data = audio_buffer->write_pos;
		}
		bytes_to_copy -= to_valid;
		audio_buffer->len -= to_valid;
	}
	pthread_mutex_unlock(&(audio_buffer->syn));

	return 0;
}
#endif
