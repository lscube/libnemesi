/* * 
 *  ./decoder/set_audiodev.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

int set_audiodev(void)
{
	int audio_fd;
	int format;
	int result;
	audio_buf_info info;

/*
	if ( (audio_fd=open("prova.wav", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR )) == -1){
		uiprintf("Could not open the file\n");
	}

*/	

	if ( (audio_fd=open("/dev/dsp", O_WRONLY | O_NONBLOCK, 0)) == -1){
		uiprintf("Could not open /dev/dsp\n");
		return -1;
	}

	format=0x7FFF000D;
	ioctl(audio_fd,SNDCTL_DSP_SETFRAGMENT, &format);
	ioctl(audio_fd,SNDCTL_DSP_GETOSPACE, &info);
	uiprintf("\nFrag: %d\nFrag total: %d\nFrag Size: %d\n", info.fragments, info.fragstotal, info.fragsize);

	ioctl(audio_fd,SNDCTL_DSP_GETCAPS, &format);
	if (format & DSP_CAP_TRIGGER) {
		format= ~PCM_ENABLE_OUTPUT;
		ioctl(audio_fd, SNDCTL_DSP_SETTRIGGER, &format);
	}
	
	if ( (result=ioctl(audio_fd, SNDCTL_DSP_GETFMTS, &format)) == -1 ){
		uiprintf("Could not get formats supported\n");
		return -1;
	}
	if (format & AFMT_S16_NE){
		format=AFMT_S16_NE;
		if ( (result=ioctl(audio_fd, SNDCTL_DSP_SETFMT, &format)) == -1 ){
			uiprintf("Could not set format\n");
			return -1;
		}
	} else {
		uiprintf("Format 16 bit signed non supported: exiting...");
		return -1;
	}
	format=2;
	if ( (result=ioctl(audio_fd, SNDCTL_DSP_CHANNELS, &format)) == -1 ){
		uiprintf("Could not set format\n");
		return -1;
	}
	uiprintf("\nChannels: %d\n", format);
	format=44100;
	if ( (result=ioctl(audio_fd, SNDCTL_DSP_SPEED, &format)) == -1 ){
		uiprintf("Could not set format\n");
		return -1;
	}
	uiprintf("Speed: %d\n", format);

	return audio_fd;
}
