#include <nemesi/audio.h>

#ifndef HAVE_SDL
#include <stdlib.h>

#include <sys/ioctl.h>
#include <sys/soundcard.h>

/* void enable_output(int audio_fd) */
void enable_output(int enable)
{
	int audio_fd = global_audio_buffer->audio_fd;
	int format;

	ioctl(audio_fd,SNDCTL_DSP_GETCAPS, &format);
	if (format & DSP_CAP_TRIGGER) {
		format= (enable ? PCM_ENABLE_OUTPUT : ~PCM_ENABLE_OUTPUT);
		ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &format);
	}
}
#endif
