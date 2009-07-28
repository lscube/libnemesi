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

#include "bufferpool/bufferpool.h"

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
    //podel(po, index);
    //bpfree(bp, index);

    pthread_mutex_lock(&(po->po_mutex));
    pthread_mutex_lock(&(bp->fl_mutex));

    if (po->pobuff[index].next != -1)
        po->pobuff[po->pobuff[index].next].prev =
            po->pobuff[index].prev;
    else
        po->potail = po->pobuff[index].prev;
    if (po->pobuff[index].prev != -1)
        po->pobuff[po->pobuff[index].prev].next =
            po->pobuff[index].next;
    else
        po->pohead = po->pobuff[index].next;

    po->pocount--;

    bp->freelist[index] = bp->flhead;
    bp->flhead = index;
    bp->flcount--;
    memset(bp->bufferpool + index, 0, sizeof(bp_slot));

    pthread_cond_signal(&(bp->cond_full));
    pthread_mutex_unlock(&(bp->fl_mutex));
    pthread_mutex_unlock(&(po->po_mutex));

    return 0;
}
