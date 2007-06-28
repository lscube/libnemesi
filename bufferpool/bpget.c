/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
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

    /*
       if (bp->flhead == -1) {
       fprintf(stderr, "\n\tFreelist empty\n");
       return -1;
       }
     */

    pthread_mutex_lock(&(bp->fl_mutex));
    while (bp->flhead == -1)
        pthread_cond_wait(&(bp->cond_full), &(bp->fl_mutex));
    offset = bp->flhead;
    bp->flhead = bp->freelist[bp->flhead];
    bp->flcount++;
    pthread_mutex_unlock(&(bp->fl_mutex));

    return offset;
}
