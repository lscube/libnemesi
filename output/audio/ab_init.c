/* * 
 *  ./decoder/audio/ab_init.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:12 $
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

struct audio_buff *ab_init()
{
	struct audio_buff *buff;
	pthread_mutexattr_t mutex_attr;
	int n;

	if ( (buff = (struct audio_buff *)malloc(sizeof(struct audio_buff))) ==NULL ) {
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}
/*
	if ( ((buff->audio_data) = (uint8 *)malloc(AUDIO_BUFF_SIZE*sizeof(uint8))) ==NULL ) {
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}
*/
	buff->read_pos=buff->write_pos=buff->valid_data=buff->len=0;

#if 1
	if ((n = pthread_mutexattr_init(&mutex_attr)) > 0)
		return NULL;
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if ((n = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED)) > 0)
		return NULL;
#endif
#endif
	if ((n = pthread_mutex_init(&(buff->syn), &mutex_attr)) > 0)
		return NULL;

	ab_get(0, buff);

#ifndef HAVE_SDL
	buff->audio_fd = -1;
#endif

	return buff;
}
