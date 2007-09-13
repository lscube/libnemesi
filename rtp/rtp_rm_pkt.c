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

inline int rtp_rm_pkt(rtp_ssrc * stm_src)
{
    return bprmv(&(stm_src->rtp_sess->bp), &(stm_src->po),
             stm_src->po.potail);
}

static void socket_clear(rtp_ssrc * stm_src)
{
    rtp_session * rtp_sess = stm_src->rtp_sess;
    char buffer[BP_SLOT_SIZE];
    fd_set readset;
    struct timeval timeout;

    memset(&timeout, 0, sizeof(struct timeval));

    while(1) {
        FD_ZERO(&readset);
        FD_SET(rtp_sess->transport.RTP.sock.fd, &readset);

        select(rtp_sess->transport.RTP.sock.fd + 1, &readset, NULL, NULL, &timeout);

        if (FD_ISSET(rtp_sess->transport.RTP.sock.fd, &readset))
            recvfrom(rtp_sess->transport.RTP.sock.fd, buffer, BP_SLOT_SIZE, 0,
                         NULL, NULL);
        else
            break;
    }
}

/**
  * Clears the playoutbuffer and the recv buffer for the given source
  */
void rtp_rm_all_pkts(rtp_ssrc * stm_src)
{
    playout_buff * po = &(stm_src->po);
    buffer_pool * bp = &(stm_src->rtp_sess->bp);

    //Clear the RECV BUFFER
    socket_clear(stm_src);

    //Clear PLAYOUTBUFFER and Bufferpool
    pthread_mutex_lock(&(po->po_mutex));
    pthread_mutex_lock(&(bp->fl_mutex));
    while(po->potail >= 0) {
        int index = po->potail;

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
    }

    pthread_cond_signal(&(bp->cond_full));
    pthread_mutex_unlock(&(bp->fl_mutex));
    pthread_mutex_unlock(&(po->po_mutex));
}
