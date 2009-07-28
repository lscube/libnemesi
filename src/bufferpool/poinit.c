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
 * \brief Inizializza il Buffer di Playout.
 *
 * \param po Il puntatore al Buffer di Playout.
 * \param bp Il puntatore il Buffer Pool.
 * \return 0
 * \see poadd
 * \see podel
 * \see bufferpool.h
 * */
int poinit(playout_buff * po, buffer_pool * bp)
{
    pthread_mutexattr_t mutex_attr;
    pthread_condattr_t cond_attr;
    int i;

    po->bufferpool = &(bp->bufferpool);
    po->pohead = po->potail = -1;
    po->cycles = 0;
    po->pocount = 0;

    if ((i = pthread_mutexattr_init(&mutex_attr)) > 0)
        return i;
#if 0
#ifdef    _POSIX_THREAD_PROCESS_SHARED
    if ((i =
         pthread_mutexattr_setpshared(&mutex_attr,
                      PTHREAD_PROCESS_SHARED)) > 0)
        return i;
#endif
#endif
    if ((i = pthread_mutex_init(&(po->po_mutex), &mutex_attr)) > 0)
        return i;

    // cond initialization
    if ((i = pthread_condattr_init(&cond_attr)) > 0)
        return i;
//      if ( (i = pthread_cond_init(&(po->cond_empty), &cond_attr) ) > 0)
//              return i;

    return 0;
}
