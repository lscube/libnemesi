/* * 
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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
int podel(playout_buff * po, int index)
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

    po->pocount--;

    pthread_mutex_unlock(&(po->po_mutex));

    return 0;
}
