/* * 
 *  ./bufferpool/bpfree.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:46 $
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
* \brief Restituisce uno slot alla Free List.
*
* Dopo aver rimosso un elemento dal Buffer di Playout, tramite la bpdel,
* inserisce in testa alla Free List l'indice dell'elemento liberato.
*
* \param bp Il puntatore al Buffer Pool corrente.
* \param index L'indice dello slot da liberare.
* \return 0
* \see podel
* \see bufferpool.h
* */
int bpfree(buffer_pool * bp, int index)
{
	pthread_mutex_lock(&(bp->fl_mutex));
		bp->freelist[index] = bp->flhead;
		bp->flhead = index;
		bp->flcount--;
	pthread_mutex_unlock(&(bp->fl_mutex));

	return 0;
}
