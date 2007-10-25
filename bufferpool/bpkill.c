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
* \brief Funzione per la chiusura della libreria Bufferpool.
*
* Libera la memoria precedentemente allocata per il Buffer Pool. Da questo
* momento in poi il Bufferpool non è più accessibile.
*
* \param bp Il puntatore al Buffer Pool corrente.
* \return 0
* \see bpinit
* \see bufferpool.h
* */
int bpkill(buffer_pool * bp)
{
    free(bp->bufferpool);
    bp->bufferpool = NULL;
    return 0;
}
