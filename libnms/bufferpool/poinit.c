/* * 
 *  $Id:poinit.c 267 2006-01-12 17:19:45Z shawill $
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
 * \brief Inizializza il Buffer di Playout.
 *
 * \param po Il puntatore al Buffer di Playout.
 * \param bp Il puntatore il Buffer Pool.
 * \return 0
 * \see poadd
 * \see podel
 * \see bufferpool.h
 * */
int poinit(playout_buff *po, buffer_pool *bp)
{
	pthread_mutexattr_t mutex_attr;
	pthread_condattr_t cond_attr;
	int i;

	po->bufferpool=&(bp->bufferpool);
	po->pohead = po->potail = -1;
	po->cycles = 0;

	if ((i = pthread_mutexattr_init(&mutex_attr)) > 0)
		return i;
#if 0
#ifdef	_POSIX_THREAD_PROCESS_SHARED
	if ((i = pthread_mutexattr_setpshared(&mutex_attr, PTHREAD_PROCESS_SHARED)) > 0)
		return i;
#endif
#endif
	if ((i = pthread_mutex_init(&(po->po_mutex), &mutex_attr)) > 0)
		return i;
		
	// cond initialization
	if ( (i = pthread_condattr_init(&cond_attr) ) > 0)
		return i;
	if ( (i = pthread_cond_init(&(po->cond_empty), &cond_attr) ) > 0)
		return i;

	return 0;
}
