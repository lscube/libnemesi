/* * 
 *  ./bufferpool/bpget.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:47 $
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
* \brief Restituisce uno slot di memoria libero dal Buffer Pool.
*
* Prende dalla testa della Free List l'indice di uno slot libero
* dal quale si puo' ricavare il puntatore all'area di memoria
* da utilizzare.
*
* \param bp Il puntatore al Buffer Pool corrente.
* \return L'indice dello slot libero nel vettore del Buffer Pool.
* \see bprmv
* \see bufferpool.h
* */
int bpget(buffer_pool * bp)
{
	int offset;

	if (bp->flhead == -1)
		return -1;

	pthread_mutex_lock(&(bp->fl_mutex));
		offset = bp->flhead;
		bp->flhead = bp->freelist[bp->flhead];
		bp->flcount++;
	pthread_mutex_unlock(&(bp->fl_mutex));

	return offset;
}
