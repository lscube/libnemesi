#include <nemesi/audio.h>

#ifndef HAVE_SDL
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/soundcard.h>
#endif

/* int get_sy_buff(struct audio_buff *audio_buffer) */
int get_sys_buff(void)
{
#ifndef HAVE_SDL
	audio_buf_info info;
#endif
	float sys_buff;
	struct audio_buff *audio_buffer = global_audio_buffer;

#ifdef HAVE_SDL
	sys_buff=(float)audio_buffer->len/(float)AUDIO_BUFF_SIZE;
/*	fprintf(stderr, "System buffer: %4.1f %% \r", (100.0*sys_buff) ); */

	sys_buff=sys_buff_size(&sys_buff);
	if(sys_buff<0.2) {
		/* fprintf(stderr, "\n***\tSystem buffer empty: %f -> required %d Fast Cicles\n", sys_buff, (int)((0.2-sys_buff)*50)); */
		/* return (int)((0.5-sys_buff)*50); / 0.5-sys_buff/0.02 -> 0.5-sys_buff/20ms */
		return 2;
	}
#else
	ioctl(audio_buffer->audio_fd,SNDCTL_DSP_GETOSPACE, &info);
/*	fprintf(stderr, "System buffer: %4.1f %% \r", (100.0-(100.0*(double)info.bytes/(double)(info.fragsize*info.fragstotal))) ); */
	sys_buff=(1.0-((float)info.bytes/(float)(info.fragsize*info.fragstotal)));
	sys_buff=sys_buff_size(&sys_buff);

	if((info.fragsize*info.fragstotal-info.bytes < 2*info.fragsize)){
	/*if((info.bytes > 2*info.fragsize)){*/
		fprintf(stderr, "\nfast cycle !!!%d\n", (info.bytes/info.fragsize)/2);
		return 2;
	}
#endif

	return 0;
}
