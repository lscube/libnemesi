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

#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <nemesi/comm.h>
#include <nemesi/utils.h>
#include <nemesi/audio_drivers.h>
#include <nemesi/audio_driver.h>
#include <nemesi/audio_format.h>
#include <nemesi/audio.h>

#define DEF_AUDIODEV "/dev/dsp"

static NMSADrvInfo info = {
	"Open Sound System audio driver",
	"oss",
	"Open Media Streaming Project Team",
	""
};

NMS_LIB_AUDIO(oss);

static struct oss_priv_s {
	int audiofd;
	char *audiodev;
	NMSAudioBuffer *audio_buffer;
} oss_priv;

static uint32 oss_init(const char *arg)
{
	if (arg)
		oss_priv.audiodev = strdup(arg);
	else
		oss_priv.audiodev = strdup(DEF_AUDIODEV);
/*
	if ( (audio_fd=open("prova.wav", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR )) == -1){
		nmserror("Could not open the file");
	}

*/	

	if ( (oss_priv.audiofd=open(oss_priv.audiodev, O_WRONLY | O_NONBLOCK, 0)) == -1)
		return nmserror("Could not open %s", oss_priv.audiodev);
	else
		nmsprintf(1, "OSS Audio device %s opened\n", oss_priv.audiodev);
	// TODO: probably we have to remove NON BLOCKING flag

	return 0;
}

static uint32 init(uint32 rate, uint8 channels, uint32 format, uint32 flags, const char *arg)
{
	int req_format;
	int result;
	audio_buf_info info;

	if ( oss_init(arg) )
		return 1;
	
	// Audio Buffer initialization
	if ( (oss_priv.audio_buffer=ab_init(AUDIO_BUFF_SIZE)) == NULL )
		return nmserror("[OSS] Failed while initializing Audio Buffer");

	nmsprintf(3, "[OSS] Requested Rate is: %d\n", rate);
	nmsprintf(3, "[OSS] Requested Channel number is: %d\n", channels);
	nmsprintf(3, "[OSS] Requested Format is: %d\n", format);
	// TODO; init card
	req_format=0x7FFF000D;
	// req_format=0x7FFF000A;
	if (ioctl(oss_priv.audiofd, SNDCTL_DSP_SETFRAGMENT, &req_format))
		return nmserror("[OSS] Could not set dsp fragments");;
	if (ioctl(oss_priv.audiofd, SNDCTL_DSP_GETOSPACE, &info))
		return nmserror("[OSS] Could not get hw buffer parameters");;
	nmsprintf(2, "\nFrag: %d\nFrag total: %d\nFrag Size: %d\n", info.fragments, info.fragstotal, info.fragsize);

	ioctl(oss_priv.audiofd, SNDCTL_DSP_GETCAPS, &req_format);
	if (req_format & DSP_CAP_TRIGGER) {
		req_format= ~PCM_ENABLE_OUTPUT;
		ioctl(oss_priv.audiofd, SNDCTL_DSP_SETTRIGGER, &req_format);
	}
	
	// Setup format
	if ( (result=ioctl(oss_priv.audiofd, SNDCTL_DSP_GETFMTS, &req_format)) == -1 )
		nmsprintf(2, "Could not get formats supported\n");

	if (req_format & format) {
		req_format = format;
		if ( (result=ioctl(oss_priv.audiofd, SNDCTL_DSP_SETFMT, &req_format)) == -1 ) {
			return nmserror("OSS: Could not set format");
		}
	} else
		return nmserror("OSS: Format %s non supported: exiting...", audio_format_name(req_format));

	// set channels
	req_format = channels;
	if ( (result=ioctl(oss_priv.audiofd, SNDCTL_DSP_CHANNELS, &req_format)) == -1 )
		return nmserror("OSS: Could not set number of channels\n");
		
	nmsprintf(2, "Channels: %d\n", req_format);
	req_format = rate;
	if ( (result=ioctl(oss_priv.audiofd, SNDCTL_DSP_SPEED, &req_format)) == -1 )
		return nmserror("Could not set rate");
	
	nmsprintf(2, "Sample rate: %d\n", req_format);

	return 0;
}

static uint32 control(uint32 cmd, void *arg)
{
	audio_buf_info info;

	switch(cmd) {
		case ACTRL_GET_SYSBUF:
			ioctl(oss_priv.audiofd, SNDCTL_DSP_GETOSPACE, &info);
			*((float *)arg) = (1.0-((float)info.bytes/(float)(info.fragsize*info.fragstotal)));
			return 0;
			break;
		default:
			return -1;
			break;
	}

	return 0;
}

static uint8 *get_buff(uint32 len)
{
	return ab_get(len);
}

static uint32 play_buff(uint8 *data, uint32 len)
{
	NMSAudioBuffer *audio_buffer = oss_priv.audio_buffer;
	int audiofd = oss_priv.audiofd;
	audio_buf_info info;
	uint32 bytes_to_copy, to_valid, prev_to_valid;
	int32 len_in_frags;

	pthread_mutex_lock(&(audio_buffer->syn));
	ioctl(audiofd,SNDCTL_DSP_GETOSPACE, &info);
	len_in_frags = audio_buffer->len/info.fragsize;
	bytes_to_copy = (min(len_in_frags, info.fragments))*info.fragsize;
	while (bytes_to_copy > 0) {
		prev_to_valid = to_valid;
		to_valid = min(bytes_to_copy,(audio_buffer->valid_data - audio_buffer->read_pos));
		write(audiofd, (audio_buffer->audio_data + audio_buffer->read_pos), to_valid);
		if ( (audio_buffer->read_pos + to_valid) < audio_buffer->valid_data )
			audio_buffer->read_pos += to_valid;
		else {
			audio_buffer->read_pos = 0;
			audio_buffer->valid_data = audio_buffer->write_pos;
		}
		bytes_to_copy -= to_valid;
		audio_buffer->len -= to_valid;
		pthread_cond_signal(&(audio_buffer->cond_full));
	}
	pthread_mutex_unlock(&(audio_buffer->syn));

	return 0;
}

static void audio_pause(void)
{
	int audiofd = oss_priv.audiofd;
	int format;

	ioctl(audiofd,SNDCTL_DSP_GETCAPS, &format);
	if (format & DSP_CAP_TRIGGER) {
		format= ~PCM_ENABLE_OUTPUT;
		ioctl(audiofd, SNDCTL_DSP_SETTRIGGER, &format);		
	}	

	return;
}

static void audio_resume(void)
{
	int audiofd = oss_priv.audiofd;
	int format;

	ioctl(audiofd,SNDCTL_DSP_GETCAPS, &format);
	if (format & DSP_CAP_TRIGGER) {
		format= PCM_ENABLE_OUTPUT;
		ioctl(audiofd, SNDCTL_DSP_SETTRIGGER, &format);		
	}

	return;
}

static void reset(void)
{
	NMSAudioBuffer *ab = oss_priv.audio_buffer;

	// reset audio buffer
	ab->len = ab->read_pos = ab->write_pos = ab->valid_data = 0;

	return;
}

static void uninit(void)
{
	NMSAudioBuffer *ab = oss_priv.audio_buffer;

	if (ab) {
		ab_uninit(ab);
		oss_priv.audio_buffer = NULL;
	}
	if((oss_priv.audiofd) > 0) 
		close(oss_priv.audiofd);

	nmsprintf(1, "OSS Audio closed\n");

	return;
}

