/* * 
 *  $Id:bprmv.c 267 2006-01-12 17:19:45Z shawill $
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
 * \brief Rimuove uno slot dalla coda del Buffer di Plaout di Rete.
 *
 * Si occupa di chiamare la funzione \c podel per la cancellazione dell'elemento dalla coda di playout
 * e la funzione \c bpfree per l'eliminazione dal vettore del Bufferpool.
 *
 * \param bp puntatore al vettore del Buffer Pool corrente
 * \param po puntatore alla lista del Buffer di Playout.
 * \param index indice dell'elemento da rimuovere.
 * \return 0
 * \see podel
 * \see bpfree
 * \see bufferpool.h
 * */
int bprmv(buffer_pool * bp, playout_buff * po, int index)
{
	podel(po, index);
	bpfree(bp, index);
	pthread_cond_signal(&(bp->cond_full));

	return 0;
}
