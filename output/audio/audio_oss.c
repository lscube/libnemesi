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
	nms_audio_buffer *audio_buffer;
	int capabilities;
	uint32 freq;
	uint32 channels;
	uint8 bytes_x_sample;
	uint32 format;
	double last_pts;
} oss_priv;

static uint32 oss_init(const char *arg)
{
	if (arg)
		oss_priv.audiodev = strdup(arg);
	else
		oss_priv.audiodev = strdup(DEF_AUDIODEV);
/*
	if ( (audio_fd=open("prova.wav", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR )) == -1){
		nms_printf(NMSML_ERR, "Could not open the file\n");
	}

*/

	if ((oss_priv.audiofd =
	     open(oss_priv.audiodev, O_WRONLY | O_NONBLOCK, 0)) == -1)
		return nms_printf(NMSML_ERR, "Could not open %s\n",
				  oss_priv.audiodev);
	else
		nms_printf(NMSML_NORM, "OSS Audio device %s opened\n",
			   oss_priv.audiodev);
	// TODO: probably we have to remove NON BLOCKING flag

	return 0;
}

static uint32 init(uint32 * rate, uint8 * channels, uint32 * format,
		   uint32 buff_ms, uint32 flags, const char *arg)
{
	int req_format;
	int result;
	audio_buf_info info;
	uint32 buff_size;

	if (oss_init(arg))
		return 1;

	oss_priv.last_pts = 0;

	switch (*format) {
	case AFMT_U8:
	case AFMT_S8:
		oss_priv.bytes_x_sample = 1;
		break;
	case AFMT_S16_LE:
	case AFMT_S16_BE:
	case AFMT_U16_LE:
	case AFMT_U16_BE:
		oss_priv.bytes_x_sample = 2;
		break;
	default:
		return nms_printf(NMSML_ERR,
				  "SDL: Unsupported audio format: %s (0x%x).\n",
				  audio_format_name(*format), *format);
		break;
	}

	// Audio Buffer initialization
	if (!buff_ms) {
		buff_size = AUDIO_BUFF_SIZE;
		nms_printf(NMSML_DBG1, "Setting default audio system buffer\n");
	} else
		buff_size =
		    buff_ms * (*rate) * (*channels) * oss_priv.bytes_x_sample /
		    1000;
	if ((oss_priv.audio_buffer = ab_init(buff_size)) == NULL)
		return nms_printf(NMSML_FATAL,
				  "[OSS] Failed while initializing Audio Buffer\n");
	nms_printf(NMSML_DBG1, "Audio system buffer: %u\n", buff_size);

	nms_printf(NMSML_DBG1, "[OSS] Requested Rate is: %d\n", *rate);
	nms_printf(NMSML_DBG1, "[OSS] Requested Channel number is: %d\n",
		   *channels);
	nms_printf(NMSML_DBG1, "[OSS] Requested Format is: %d\n", *format);

	ioctl(oss_priv.audiofd, SNDCTL_DSP_GETCAPS, &oss_priv.capabilities);
	// TODO; init card
	req_format = 0x7FFF000D;
	// req_format=0x7FFF000A;
	if (ioctl(oss_priv.audiofd, SNDCTL_DSP_SETFRAGMENT, &req_format))
		return nms_printf(NMSML_ERR,
				  "[OSS] Could not set dsp fragments\n");;
	if (ioctl(oss_priv.audiofd, SNDCTL_DSP_GETOSPACE, &info))
		return nms_printf(NMSML_ERR,
				  "[OSS] Could not get hw buffer parameters\n");
	nms_printf(NMSML_VERB, "\nFrag: %d\nFrag total: %d\nFrag Size: %d\n",
		   info.fragments, info.fragstotal, info.fragsize);

	if (oss_priv.capabilities & DSP_CAP_TRIGGER) {
		req_format = ~PCM_ENABLE_OUTPUT;
		ioctl(oss_priv.audiofd, SNDCTL_DSP_SETTRIGGER, &req_format);
	}
	// Setup format
	if ((result =
	     ioctl(oss_priv.audiofd, SNDCTL_DSP_GETFMTS, &req_format)) == -1)
		nms_printf(NMSML_VERB, "Could not get formats supported\n");
	if (req_format & *format) {
		oss_priv.format = *format;
		if ((result =
		     ioctl(oss_priv.audiofd, SNDCTL_DSP_SETFMT,
			   &oss_priv.format)) == -1) {
			return nms_printf(NMSML_ERR,
					  "OSS: Could not set format\n");
		}
	} else
		return nms_printf(NMSML_ERR,
				  "OSS: Format %s not supported: exiting...\n",
				  audio_format_name(oss_priv.format));

	// set channels
	oss_priv.channels = *channels;
	if ((result =
	     ioctl(oss_priv.audiofd, SNDCTL_DSP_CHANNELS,
		   &oss_priv.channels)) == -1)
		return nms_printf(NMSML_ERR,
				  "OSS: Could not set number of channels\n");
	nms_printf(NMSML_VERB, "Channels: %d\n", oss_priv.channels);

	oss_priv.freq = *rate;
	if ((result =
	     ioctl(oss_priv.audiofd, SNDCTL_DSP_SPEED, &oss_priv.freq)) == -1)
		return nms_printf(NMSML_ERR, "Could not set rate\n");
	nms_printf(NMSML_VERB, "Sample rate: %d\n", oss_priv.freq);

	// set output parameters
	*rate = oss_priv.freq;
	*channels = oss_priv.channels;
	*format = oss_priv.format;

	return 0;
}

static uint32 control(uint32 cmd, void *arg)
{
	audio_buf_info info;
	int bytes;

	switch (cmd) {
	case ACTRL_GET_SYSBUF:
		ioctl(oss_priv.audiofd, SNDCTL_DSP_GETOSPACE, &info);
		*((float *) arg) =
		    (1.0 -
		     ((float) info.bytes /
		      (float) (info.fragsize * info.fragstotal)));
		break;
	case ACTRL_GET_ELAPTM:
		if (oss_priv.last_pts) {
#ifdef SNDCTL_DSP_GETODELAY
			if (ioctl(oss_priv.audiofd, SNDCTL_DSP_GETODELAY, &bytes) == -1) {	// XXX: try using GETODELAY
#endif
				if (ioctl(oss_priv.audiofd, SNDCTL_DSP_GETOSPACE, &info) == -1)	// XXX: try using GETOSPACE
					return -1;
				bytes =
				    info.fragsize * info.fragstotal -
				    info.bytes;
#ifdef SNDCTL_DSP_GETODELAY
			}
#endif
			*((double *) arg) =
			    oss_priv.last_pts -
			    ((double)
			     (bytes +
			      oss_priv.audio_buffer->buff_size /*len */ ) *
			     1000.0) /
			    (double) (oss_priv.freq * oss_priv.channels *
				      oss_priv.bytes_x_sample);
		} else
			*((double *) arg) = 0;
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

static uint32 play_buff(uint8 * data, uint32 len, double pts)
{
	nms_audio_buffer *audio_buffer = oss_priv.audio_buffer;
	int audiofd = oss_priv.audiofd;
	audio_buf_info info;
	uint32 bytes_to_copy, to_valid, prev_to_valid;
	int32 len_in_frags;

	pthread_mutex_lock(&(audio_buffer->syn));
	ioctl(audiofd, SNDCTL_DSP_GETOSPACE, &info);
	len_in_frags = audio_buffer->len / info.fragsize;
	bytes_to_copy = (min(len_in_frags, info.fragments)) * info.fragsize;
	while (bytes_to_copy > 0) {
		prev_to_valid = to_valid;
		to_valid =
		    min(bytes_to_copy,
			(audio_buffer->valid_data - audio_buffer->read_pos));
		write(audiofd,
		      (audio_buffer->audio_data + audio_buffer->read_pos),
		      to_valid);
		if ((audio_buffer->read_pos + to_valid) <
		    audio_buffer->valid_data)
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

	oss_priv.last_pts = max(oss_priv.last_pts, pts);

	return 0;
}

static void audio_pause(void)
{
	int audiofd = oss_priv.audiofd;
	int format;

	ioctl(audiofd, SNDCTL_DSP_GETCAPS, &format);
	if (format & DSP_CAP_TRIGGER) {
		format = ~PCM_ENABLE_OUTPUT;
		ioctl(audiofd, SNDCTL_DSP_SETTRIGGER, &format);
	}

	return;
}

static void audio_resume(void)
{
	int audiofd = oss_priv.audiofd;
	int format;

	ioctl(audiofd, SNDCTL_DSP_GETCAPS, &format);
	if (format & DSP_CAP_TRIGGER) {
		format = PCM_ENABLE_OUTPUT;
		ioctl(audiofd, SNDCTL_DSP_SETTRIGGER, &format);
	}

	return;
}

static void reset(void)
{
	nms_audio_buffer *ab = oss_priv.audio_buffer;

	// reset audio buffer
	ab->len = ab->read_pos = ab->write_pos = ab->valid_data = 0;
	oss_priv.last_pts = 0;

	return;
}

static void uninit(void)
{
	nms_audio_buffer *ab = oss_priv.audio_buffer;

	if (ab) {
		ab_uninit(ab);
		oss_priv.audio_buffer = NULL;
	}
	if ((oss_priv.audiofd) > 0)
		close(oss_priv.audiofd);

	nms_printf(NMSML_NORM, "OSS Audio closed\n");

	return;
}
