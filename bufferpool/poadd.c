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

#include <nemesi/rtp.h>
#include <nemesi/bufferpool.h>

/*!
* \brief Inserisce un elemento nel Buffer di Playout.
*
* L'inserimento e' ordinato secondo il numero di sequenza del pacchetto RTP.
* Si tratta di un normale inserimento in una lista doppio linkata con i
* collegameti effettuati tramite gli indici del vettore.
*
* \param po Il Buffer Pool corrente.
* \param index L'indice dello slot allocato dalla poget.
* \param cycles I cicli del campo \c SEQ dei pacchetti RTP.
* \return 0
* \see bpget
* \see podel
* \see bufferpool.h
* */
int poadd(playout_buff *po, int index, uint32 cycles)
{
	int i;
	uint32 cseq;

	pthread_mutex_lock(&(po->po_mutex));

	i = po->pohead;

	cseq = (uint32)ntohs(((rtp_pkt *) (*(po->bufferpool) + index))->seq) + cycles;
	while ((i != -1) && ((uint32)ntohs(((rtp_pkt *) (*(po->bufferpool) + i))->seq) + po->cycles > cseq)) {
		i = po->pobuff[i].next;
	}
	if ( (i != -1) && (cseq == ((uint32)ntohs(((rtp_pkt *) (*(po->bufferpool) + i))->seq) + po->cycles)) ) {
		pthread_mutex_unlock(&(po->po_mutex));
		return 1;
	}
	if (i == po->pohead) {	/* inserimento in testa */
		po->pobuff[index].next = i;
		po->pohead = index;
		if( i == -1 )
			po->potail=index;
		else
			po->pobuff[i].prev = index;
		po->pobuff[index].prev = -1;
		po->cycles=cycles;
	} else {		
		if (i == -1) { /* inserimento in coda*/
			i = po->potail;
			po->potail=index;
		}
		else	/* inserimento */
			po->pobuff[po->pobuff[i].next].prev=index;		
		
		po->pobuff[index].next = po->pobuff[i].next;
		po->pobuff[i].next = index;
		po->pobuff[index].prev = i;
		
		pthread_mutex_unlock(&(po->po_mutex));
		return 2;
	}

	pthread_mutex_unlock(&(po->po_mutex));

	return 0;
}
