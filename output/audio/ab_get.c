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

#include <nemesi/audio.h>

#define WAIT_IF_FULL 10 /* millisecondi di sleep se il buffer è pieno */

uint8 *ab_get(uint32 len, ...)
{
	static struct audio_buff *audio_buffer = NULL;
	va_list ap;
	struct timeval tv_sleep = {0, 1000};
	
	if ( (len > 0) && (audio_buffer)) {
		while(1) {
			pthread_mutex_lock(&(audio_buffer->syn));
			if ( (audio_buffer->write_pos >= audio_buffer->read_pos) ) {
				if ((audio_buffer->write_pos + len) </*=*/ AUDIO_BUFF_SIZE ) {
					audio_buffer->write_pos += len;
					audio_buffer->valid_data = audio_buffer->write_pos;
					audio_buffer->len += len;
					pthread_mutex_unlock(&(audio_buffer->syn));
					return &audio_buffer->audio_data[audio_buffer->write_pos-len];
				} else if ( len < audio_buffer->read_pos ) {
					audio_buffer->write_pos = len;
					audio_buffer->len += len;
					pthread_mutex_unlock(&(audio_buffer->syn));
					return audio_buffer->audio_data;
				}
			} else if( (audio_buffer->write_pos + len) </*=*/ audio_buffer->read_pos ) {
				audio_buffer->write_pos += len;
				audio_buffer->len += len;
				pthread_mutex_unlock(&(audio_buffer->syn));
				return &audio_buffer->audio_data[audio_buffer->write_pos-len];
			}
			pthread_mutex_unlock(&(audio_buffer->syn));
			uiprintf("No more space in System Buffer\n");
			tv_sleep.tv_sec = 0;
			tv_sleep.tv_usec = WAIT_IF_FULL*1000;
			select(0, NULL, NULL, NULL, &tv_sleep);
		}
	} else if ( len == 0 ){
		/* audio buffer pointer assignement */
		va_start(ap, len);
		audio_buffer = va_arg(ap, struct audio_buff *);
	} else {
		uiprintf("\nError in <ab_get> function\n");
	}
	return NULL;
}
