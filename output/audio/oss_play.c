/* * 
 *  ./output/audio/oss_play.c: $Revision: 1.2 $ -- $Date: 2003/01/15 11:18:00 $
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
