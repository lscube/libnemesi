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

#include <nemesi/rtp.h>

/*! \brief This function returns a pointer to next packet in the bufferpool for
 * given playout buffer.
 * WARNING: the pointer returned is the memory space of the slot inside buffer pool:
 * Once the packet is decoded it must be removed from rtp queue using \see rtp_rm_pkt.
 * WARNING: returned pointer looks at a memory space not locked by mutex. This because
 * we suppose that there is only one reader for each playout buffer.
 * We lock mutex only for potail var reading.
 * \param len this is a return parameter for lenght of pkt. NULL value is allowed:
 * in this case, we understand that you are not interested about this value.
 * shawill: this function put his dirty hands on bufferpool internals!!!
 * \return the pointer to next packet in buffer or NULL if playout buffer is empty.
 * */
rtp_pkt *rtp_get_pkt(rtp_ssrc * stm_src, size_t * len)
{
    pthread_mutex_lock(&(stm_src->po.po_mutex));
    do {
        if (stm_src->po.potail < 0) {
            pthread_mutex_unlock(&(stm_src->po.po_mutex));
            return NULL;
        }
    } while (!stm_src->rtp_sess->
         ptdefs[((rtp_pkt *) (*(stm_src->po.bufferpool) +
                      stm_src->po.potail))->pt]
         &&
         /* always true - XXX be careful if bufferpool API changes -> */
         !rtp_rm_pkt(stm_src));
    pthread_mutex_unlock(&(stm_src->po.po_mutex));

    if (len)
        *len = (stm_src->po.pobuff[stm_src->po.potail]).pktlen;
//      pthread_mutex_unlock(&(stm_src->po.po_mutex)); moved up

    return (rtp_pkt *) (*(stm_src->po.bufferpool) + stm_src->po.potail);
}
