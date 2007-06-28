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

#define RET_ERR(x)    do {\
                free(bp->bufferpool); \
                return x; \
            } while (0)

/*!
* \brief Inizializza il Buffer Pool.
*
* Alloca la memoria per il Buffer di Playout e inizializza la Free List per la gestione interna della memoria.
* Inizializza la variabile di accesso in Mutua Esclusione alla Free List.
*
* \param bp Il puntatore al Buffer Pool corrente.
* \return 1 in caso di errore, 0 altrimenti.
* \see bpkill
* \see bufferpool.h
* */
int bpinit(buffer_pool * bp)
{
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
    int i;

    if (((bp->bufferpool) =
         (bp_slot *) malloc(BP_SLOT_NUM * sizeof(bp_slot))) == NULL) {
        return 1;
    }
    memset(bp->bufferpool, 0, BP_SLOT_NUM * sizeof(bp_slot));
    for (i = 0; i < BP_SLOT_NUM; bp->freelist[i] = i + 1, i++);
    bp->freelist[BP_SLOT_NUM - 1] = -1;
    bp->flhead = 0;
    bp->flcount = 0;

    if ((i = pthread_mutexattr_init(&mutex_attr)) > 0)
        RET_ERR(i);
#if 0
#ifdef    _POSIX_THREAD_PROCESS_SHARED
    if ((i =
         pthread_mutexattr_setpshared(&mutex_attr,
                      PTHREAD_PROCESS_SHARED)) > 0)
        RET_ERR(i);
#endif
#endif
    if ((i = pthread_mutex_init(&(bp->fl_mutex), &mutex_attr)) > 0)
        RET_ERR(i);
    // cond initialization
    if ((i = pthread_condattr_init(&cond_attr)) > 0)
        RET_ERR(i);
    if ((i = pthread_cond_init(&(bp->cond_full), &cond_attr)) > 0)
        RET_ERR(i);

    return 0;
}
