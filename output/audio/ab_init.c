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

/* definizione per l'allocazione del buffer globale
 * questa definizione deve essere fatta solo in un file.
 *  */
#define GLOBAL_AUDIO_BUFFER

#include <nemesi/audio.h>

struct audio_buff *ab_init(uint32 buff_size)
{
	struct audio_buff *buff;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	// int n;

	if ( (buff = (struct audio_buff *)malloc(sizeof(struct audio_buff))) == NULL ) {
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}

	if ( ((buff->audio_data) = (uint8 *)malloc(buff_size*sizeof(uint8))) ==NULL ) {
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}

	buff->read_pos=buff->write_pos=buff->valid_data=buff->len=0;
	buff->buff_size = buff_size;

	// nutex initialization
	if (pthread_mutexattr_init(&mutex_attr) > 0)
		return NULL;
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if (pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED) > 0)
		return NULL;
#endif
	if (pthread_mutex_init(&(buff->syn), &mutex_attr) > 0)
		return NULL;

	// cond initioalization
	if (pthread_condattr_init(&cond_attr) > 0)
		return NULL;

	if (pthread_cond_init(&(buff->cond_full), &cond_attr) > 0)
		return NULL;

	ab_get(0, buff);

	return buff;
}
