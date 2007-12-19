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

#include "bufferpool.h"

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
    memset(bp->bufferpool + index, 0, sizeof(bp_slot));
    pthread_mutex_unlock(&(bp->fl_mutex));

    return 0;
}
