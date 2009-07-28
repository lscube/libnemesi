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


/** @file rtp_buffer.c
 * This file contains the functions that perform playout buffer and make
 * possible to access packets pending on the buffer.
 */

#include "nemesi/rtp.h"
#include "nemesi/rtpptdefs.h"
#include "bufferpool/bufferpool.h"

/**
 * Waits for rtp thread to be ready and reports if the stream reached the end
 * @return 0 if the stream isn't at the end, 1 if the stream reached the end
 */
int rtp_fill_buffers(rtp_thread * rtp_th)
{
    pthread_mutex_lock(&(rtp_th->syn));
    pthread_mutex_unlock(&(rtp_th->syn));

    return !rtp_th->run;
}

/**
 *  fills the frame with depacketized data (full frame or sample group) and
 *  provides optional extradata if available. The structs MUST be empty and
 *  the data delivered MUST not be freed.
 *  @param stm_src an active ssrc
 *  @param fr an empty frame structure
 *  @param config an empty buffer structure
 *  @return RTP_FILL_OK on success
 */
int rtp_fill_buffer(rtp_ssrc * stm_src, rtp_frame * fr, rtp_buff * config)
{
    rtp_pkt *pkt;
    int err;
    double ts_jump;

    /* If we did a seek, we must wait for seek reset and bufferpool clean up,
     * so wait until rtp_recv receives the first new packet and resets the bufferpool
     */
    if (stm_src->done_seek) {
        usleep(1000);
        return RTP_BUFF_EMPTY;
    }

    if (!(pkt = rtp_get_pkt(stm_src, NULL))) {
        usleep(1000);
        return RTP_BUFF_EMPTY;
    }

    fr->pt = RTP_PKT_PT(pkt);
    fr->timestamp = RTP_PKT_TS(pkt);

    ts_jump = ((double) (fr->timestamp - stm_src->ssrc_stats.lastts)) /
        (double) stm_src->rtp_sess->ptdefs[fr->pt]->rate;

/*
    if (ts_jump > 10) {
        fprintf(stderr, "Out of sync timestamp: %u - %u: %g for Payload: %d\n", fr->timestamp, stm_src->ssrc_stats.lastts, ts_jump, fr->pt);
        stm_src->ssrc_stats.lastts = fr->timestamp;
        rtp_rm_pkt(stm_src);
        return RTP_BUFF_EMPTY;
    }
*/

    fr->fps = stm_src->rtp_sess->fps;
    stm_src->ssrc_stats.lastts = fr->timestamp;

    while ((err = stm_src->rtp_sess->parsers[fr->pt] (stm_src, fr, config))
            == EAGAIN);
    /*
     * The parser can set the timestamp on its own
     */
    fr->time_sec = ((double) (fr->timestamp - stm_src->ssrc_stats.firstts)) /
        (double) stm_src->rtp_sess->ptdefs[fr->pt]->rate;
    return err;
}

/**
 * Gets the time in seconds between the first packet of the RTP stream
 * and the next one in the buffer.
 *
 * @param stm_src The source from which to get the packet
 *
 * @return The values in seconds
 */
double rtp_get_next_ts(rtp_ssrc * stm_src)
{                // TODO: calculate time using RTCP infos
    rtp_pkt *pkt;

    if (!(pkt = rtp_get_pkt(stm_src, NULL)))
        return -1;

    return ((double) (RTP_PKT_TS(pkt) - stm_src->ssrc_stats.firstts)) /
        (double) stm_src->rtp_sess->ptdefs[pkt->pt]->rate;
}

/**
 * Gets the payload type id for the next packet in the buffer
 *
 * @param stm_src The source from which to get the packet
 *
 * @return The payload type id
 */
int16_t rtp_get_next_pt(rtp_ssrc * stm_src)
{
    rtp_pkt *pkt;

    if (!(pkt = rtp_get_pkt(stm_src, NULL)))
        return RTP_BUFF_EMPTY;

    return pkt->pt;
}

/**
 *  Guess the fps based on timestamps of incoming packets.
 *  @param stm_src an active ssrc
 *  @param timestamp the time stamp of the last received packet
 */
void rtp_update_fps(rtp_ssrc * stm_src, uint32_t timestamp, unsigned pt) {
    if (timestamp != stm_src->rtp_sess->ptdefs[pt]->prev_timestamp) {
        stm_src->rtp_sess->fps =
        (double) stm_src->rtp_sess->ptdefs[pt]->rate/
        abs(timestamp - stm_src->rtp_sess->ptdefs[pt]->prev_timestamp);
        stm_src->rtp_sess->ptdefs[pt]->prev_timestamp = timestamp;
    }
}

/**
 *  Guess the fps based on what is available on the buffer.
 *  @param stm_src an active ssrc
 *  @return fps
 */
float rtp_get_fps(rtp_ssrc * stm_src)
{
    return stm_src->rtp_sess->fps;
}

/**
 * Returns a pointer to Nth packet in the bufferpool for given playout buffer.
 * WARNING: the pointer returned is the memory space of the slot inside buffer pool:
 * Once the packet is decoded it must be removed from rtp queue using @see rtp_rm_pkt.
 * WARNING: returned pointer looks at a memory space not locked by mutex. This because
 * we suppose that there is only one reader for each playout buffer.
 * We lock mutex only for potail var reading.
 * @param stm_src The source for which to get the packet
 * @param len this is a return parameter for lenght of pkt. NULL value is allowed:
 * in this case, we understand that you are not interested about this value.
 * @param pkt_num The index of the packet we want to get.
 * shawill: this function put his dirty hands on bufferpool internals!!!
 * @return the pointer to next packet in buffer or NULL if playout buffer is empty.
 * */
rtp_pkt *rtp_get_n_pkt(rtp_ssrc * stm_src, unsigned int *len, unsigned int pkt_num)
{                // TODO complete;
    int buffer_index;

    pthread_mutex_lock(&(stm_src->po->po_mutex));
    buffer_index = stm_src->po->potail;
    while ((buffer_index >= 0) && (pkt_num-- > 0))
        buffer_index = stm_src->po->pobuff[buffer_index].next;
    pthread_mutex_unlock(&(stm_src->po->po_mutex));

    if (buffer_index < 0)
        return NULL;

    if (len)
        *len = (stm_src->po->pobuff[buffer_index]).pktlen;

    return (rtp_pkt *) (*(stm_src->po->bufferpool) + buffer_index);
}

/** Returns a pointer to next packet in the bufferpool for given playout buffer.
 * WARNING: the pointer returned is the memory space of the slot inside buffer
 * pool:
 * Once the packet is decoded it must be removed from rtp queue using @see
 * rtp_rm_pkt.
 * WARNING: returned pointer looks at a memory space not locked by mutex.
 * This because
 * we suppose that there is only one reader for each playout buffer.
 * We lock mutex only for potail var reading.
 * @param stm_src The source for which to get the packet.
 * @param len this is a return parameter for lenght of pkt.
 * NULL value is allowed:
 * in this case, we understand that you are not interested about this value.
 * shawill: this function put its dirty hands on bufferpool internals!!!
 * @return the pointer to next packet in buffer or NULL if playout buffer
 * is empty.
 * */
rtp_pkt *rtp_get_pkt(rtp_ssrc * stm_src, size_t * len)
{
    int index;

    do {
        pthread_mutex_lock(&(stm_src->po->po_mutex));
        index = stm_src->po->potail;
        pthread_mutex_unlock(&(stm_src->po->po_mutex));

        if (index < 0) return NULL;
    } while (!stm_src->rtp_sess->
         ptdefs[((rtp_pkt *) (*(stm_src->po->bufferpool) +
                      index))->pt]
         &&
         /* always true - XXX be careful if bufferpool API changes -> */
         !rtp_rm_pkt(stm_src));

    if (len)
        *len = (stm_src->po->pobuff[index]).pktlen;

    return (rtp_pkt *) (*(stm_src->po->bufferpool) + index);
}

/**
 * Removes the first packet from the playout buffer
 * @param stm_src The source for which to remove the packet
 * @return 0
 */
inline int rtp_rm_pkt(rtp_ssrc * stm_src)
{
    return bprmv(stm_src->rtp_sess->bp, stm_src->po,
             stm_src->po->potail);
}

/**
 * Clears the buffer of the socket removing every pending packet
 * @param stm_src The source for which to clear the socket buffer
 */
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
 * Clears the playoutbuffer and the socket buffer for the given source
 * @param stm_src The source for which to remove all the pending packets
 */
void rtp_rm_all_pkts(rtp_ssrc * stm_src)
{
    playout_buff * po = stm_src->po;
    buffer_pool * bp = stm_src->rtp_sess->bp;

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
