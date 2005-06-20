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

#include <nemesi/bufferpool.h>

/*!
* \brief Inizializza il Buffer Pool.
*
* Alloca la memoria per il Buffer di Playout e inizializza la Free List per la gestione interna della memoria.
* Inizializza la variabile di accesso in Mutua Esclusione alla Free List.
*
* \param bp Il puntatore al Buffer Pool corrente.
* \return 1 in caso di errore, 0 altrimenti.
* \see bpkill
* \see bufferpool.h
* */
int bpinit(buffer_pool *bp)
{
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	int i;

	if (((bp->bufferpool) = (BP_SLOT *) malloc(BP_SLOT_NUM*sizeof(BP_SLOT))) == NULL) {
		return 1;
	}
	memset(bp->bufferpool, 0, BP_SLOT_NUM*sizeof(BP_SLOT));
	for (i = 0; i < BP_SLOT_NUM; bp->freelist[i] = i + 1, i++);
	bp->freelist[BP_SLOT_NUM - 1] = -1;
	bp->flhead = 0;
	bp->flcount = 0;

	// if ((i = // pthread_mutex_attr_init always returns 0
	pthread_mutexattr_init(&mutex_attr);
	// ) > 0)
	// 	return i;
#if 0
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if ((i = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED)) > 0)
		return i;
#endif
#endif
	// if ((i = // pthread_mutex_init always returns 0
	pthread_mutex_init(&(bp->fl_mutex), &mutex_attr);
	// ) > 0)
	// 	return i;
	// cond initialization
	// if ((i =  // pthread_condattr_init always returns 0
	pthread_condattr_init(&cond_attr);
	// ) > 0)
	// 	return i;

	// if ((i = // pthread_cond_init always returns 0
	pthread_cond_init(&(bp->cond_full), &cond_attr);
	// ) > 0)
	// 	return NULL;

	
	return 0;
}
