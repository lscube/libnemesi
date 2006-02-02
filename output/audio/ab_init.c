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

#define RET_ERR(ret_level, ...) { \
				nms_printf(ret_level, __VA_ARGS__ ); \
				free(buff->audio_data); \
				free(buff); \
				return NULL; \
			}

nms_audio_buffer *ab_init(uint32 buff_size)
{
	struct audio_buff *buff;
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	// int n;

	if ( (buff = (struct audio_buff *)malloc(sizeof(struct audio_buff))) == NULL ) {
		nms_printf(NMSML_FATAL, "Cannot allocate memory.\n");
		return NULL;
	}

	if ( ((buff->audio_data) = (uint8 *)malloc(buff_size*sizeof(uint8))) == NULL )
		RET_ERR(NMSML_FATAL, "Cannot allocate memory.\n")

	buff->read_pos=buff->write_pos=buff->valid_data=buff->len=0;
	buff->buff_size = buff_size;

	// nutex initialization
	if (pthread_mutexattr_init(&mutex_attr) > 0)
		RET_ERR(NMSML_FATAL, "Could not init mutex attributes!\n")
#if 0
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if (pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED) > 0)
		RET_ERR(NMSML_FATAL, "Could not set mutex attribute (PROCESS_SHARED)!\n")
		return NULL;
#endif
#endif
	if (pthread_mutex_init(&(buff->syn), &mutex_attr) > 0)
		RET_ERR(NMSML_FATAL, "Could not init mutex!\n")

	if (pthread_condattr_init(&cond_attr) > 0)
		RET_ERR(NMSML_FATAL, "Could not init condition variable attributes!\n")
	if (pthread_cond_init(&(buff->cond_full), &cond_attr) > 0)
		RET_ERR(NMSML_FATAL, "Could not init condition variable!\n")

	ab_get(0, buff);

	return buff;
}
