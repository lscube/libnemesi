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
* \brief Rimuove un elemento dal Buffer di Playout.
*
* La funzione gestisce solo la rimozione dal vettore del Buffer di Playout, ma
* non si occupa di reinserire l'elemento liberato nella free list.  Questa
* azione compete al Buffer Pool e non al Buffer di Playout, quindi dovrà essere
* effettuata tramite la funzione <tt>\ref bpfree</tt>.  La \c podel non sarà
* mai chiamata direttamente all'interno di \em NeMeSI, ma solo attraverso la
* <tt>\ref bprmv</tt>.
*
* \param po Il puntatore al Buffer di Playout corrente.
* \param index L'indice dell'elemento da rimuovere.
* \return 0
* \see bpfree
* \see bprmv
* \see bufferpool.h
* */
int podel(playout_buff *po, int index)
{

	pthread_mutex_lock(&(po->po_mutex));

	if (po->pobuff[index].next != -1)
		po->pobuff[po->pobuff[index].next].prev = po->pobuff[index].prev;
	else
		po->potail = po->pobuff[index].prev;
	if (po->pobuff[index].prev != -1)
		po->pobuff[po->pobuff[index].prev].next = po->pobuff[index].next;
	else
		po->pohead = po->pobuff[index].next;

	pthread_mutex_unlock(&(po->po_mutex));

	return 0;
}
