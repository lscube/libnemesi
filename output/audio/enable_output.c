/* * 
 *  ./output/audio/enable_output.c: $Revision: 1.2 $ -- $Date: 2003/01/15 11:18:00 $
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
