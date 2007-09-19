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

/** @file rtp_recv.c
 * This file contains the functions that perform packet reception and validity check.
 */

#include <nemesi/rtp.h>
#include <nemesi/rtpptdefs.h>

/**
 * Checks if the RTP header is valid for the given packet
 *
 * @param pkt Pointer to the packet
 * @param len Length of the packet
 *
 * @return 0 if the header is valid, 1 otherwise
 */
static int rtp_hdr_val_chk(rtp_pkt * pkt, int len)
{
    if (RTP_PAYLOAD_SIZE(pkt, len) < 0) {
//      if (len < 12) {
        nms_printf(NMSML_ERR,
               "RTP packet too small (%d: smaller than RTP header size)!!!\n",
               len);
        return 1;
    }

    if (pkt->ver != RTP_VERSION) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching version number!"
               BLANK_LINE);
        return 1;
    }
    if ((pkt->pt >= 200) && (pkt->pt <= 204)) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching payload type!"
               BLANK_LINE);
        return 1;
    }
    if ((pkt->pad)
        && (*(((uint8_t *) pkt) + len - 1) >
        (len - ((uint8_t *) (pkt->data) - (uint8_t *) pkt)))) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching lenght!"
               BLANK_LINE);
        return 1;
    }
    if ((pkt->cc)
        && (pkt->cc >
        (len - ((uint8_t *) (pkt->data) - (uint8_t *) pkt)) -
        ((*(((uint8_t *) pkt) + len - 1)) * pkt->pad))) {
        nms_printf(NMSML_WARN,
               "RTP Header not valid: mismatching CSRC count!"
               BLANK_LINE);
        return 1;
    }

    return 0;
}

/**
 * Initializes informations about sequence numbers for the given source
 * using the given seq number as the first one. And appends the source
 * to the list of active sources for the given RTP session.
 *
 * @param stm_src The source for which to initialize the sequence numbers informations
 * @param seq The first sequence number
 */
static void rtp_init_seq(rtp_ssrc * stm_src, uint16_t seq)
{
    struct rtp_ssrc_stats *stats = &(stm_src->ssrc_stats);

    stats->base_seq = seq - 1;    // FIXME: in rfc 3550 it's set to seq.
    stats->max_seq = seq;
    stats->bad_seq = RTP_SEQ_MOD + 1;
    stats->cycles = 0;
    stats->received = 0;
    stats->received_prior = 0;
    stats->expected_prior = 1;

    // our initializations
    // enqueue this SSRC in active SSRCs queue of RTP session.
    stm_src->next_active = stm_src->rtp_sess->active_ssrc_queue;
    stm_src->rtp_sess->active_ssrc_queue = stm_src;

    return;
}

/**
 * Updates informations about sequence numbers for the given source.
 * Sets max sequence number, initializes the sequence numbers and calls
 * rtp_init if it is the first received sequence number, checks for
 * misordered packets/jumps
 *
 * @param stm_src The source for which to update the sequence numbers informations
 * @param seq The sequence number to analyze
 */
static void rtp_update_seq(rtp_ssrc * stm_src, uint16_t seq)
{
    struct rtp_ssrc_stats *stats = &(stm_src->ssrc_stats);
    uint16_t udelta = seq - stats->max_seq;

    if (stats->probation) {
        if (seq == stats->max_seq + 1) {
            stats->probation--;
            stats->max_seq = seq;
            if (stats->probation == 0) {
                rtp_init_seq(stm_src, seq);
                stats->received++;
                return;
            }
        } else {
            stats->probation = MIN_SEQUENTIAL - 1;
            stats->max_seq = seq;
        }
        return;
    } else if (udelta < MAX_DROPOUT) {
        if (seq < stats->max_seq) {
            /*
             * Sequence number wrapped - count another 64k cycle.
             */
            stats->cycles += RTP_SEQ_MOD;
        }
        stats->max_seq = seq;
    } else if (udelta <= RTP_SEQ_MOD - MAX_MISORDER) {
        /* the sequence number made a very large jump */
        if (seq == stats->bad_seq) {
            rtp_init_seq(stm_src, seq);
        } else {
            stats->bad_seq = (seq + 1) & (RTP_SEQ_MOD - 1);
            return;
        }
    }            /* else {
                   duplicate or reorder packet
                   } */
    stats->received++;
    return;
}

/**
 * Reads a packet from the RTP socket. Checks if its valid,
 * creates a new source if the sender of the packet isnt already known
 * and appends it to the playout buffer of the source.
 *
 * @param rtp_sess The RTP session for which to receive the packet
 *
 * @return 0 if the packet was correctly received, 1 otherwise.
 */
int rtp_recv(rtp_session * rtp_sess)
{

    int n;
    unsigned rate;
    int slot;
    rtp_pkt *pkt;
    rtp_ssrc *stm_src;
    struct timeval now;
    unsigned transit;
    int delta;

    struct sockaddr_storage serveraddr;
    nms_sockaddr server =
        { (struct sockaddr *) &serveraddr, sizeof(serveraddr) };

    if ((slot = bpget(&(rtp_sess->bp))) < 0) {
        nms_printf(NMSML_VERB,
               "No more space in Playout Buffer!" BLANK_LINE);
        return 1;
    }

    if ((n = recvfrom(rtp_sess->transport.RTP.sock.fd,
                      &((rtp_sess->bp).bufferpool[slot]),
                      BP_SLOT_SIZE, 0, server.addr, &server.addr_len)) == -1) {
        switch (errno) {
        case EBADF:
            nms_printf(NMSML_ERR,
                   "RTP recvfrom: invalid descriptor\n");
            break;
        case ENOTSOCK:
            nms_printf(NMSML_ERR, "RTP recvfrom: not a socket\n");
            break;
        case EINTR:
            nms_printf(NMSML_ERR,
                   "RTP recvfrom: The receive was interrupted by delivery"
                   " of a signal\n");
            break;
        case EFAULT:
            nms_printf(NMSML_ERR,
                   "RTP recvfrom: The buffer points outside userspace\n");
            break;
        case EINVAL:
            nms_printf(NMSML_ERR,
                   "RTP recvfrom: Invalid argument passed.\n");
            break;
        default:
            nms_printf(NMSML_ERR, "in RTP recvfrom\n");
            break;
        }
        return 1;
    }
    gettimeofday(&now, NULL);

    pkt = (rtp_pkt *) & ((rtp_sess->bp).bufferpool[slot]);

    if (rtp_hdr_val_chk(pkt, n)) {
        nms_printf(NMSML_NORM, "RTP header validity check FAILED!\n");
        bpfree(&(rtp_sess->bp), slot);
        return 0;
    }

    switch (rtp_ssrc_check (rtp_sess, RTP_PKT_SSRC(pkt),
                            &stm_src, &server, RTP)) {
    case SSRC_KNOWN:
        if (stm_src->done_seek) {
            stm_src->ssrc_stats.probation = 0;
            stm_src->ssrc_stats.max_seq = RTP_PKT_SEQ(pkt);
            stm_src->ssrc_stats.firstts = RTP_PKT_TS(pkt);
            stm_src->ssrc_stats.firsttv = now;
            stm_src->ssrc_stats.jitter = 0;

            stm_src->ssrc_stats.base_seq = RTP_PKT_SEQ(pkt) - 1;    // FIXME: in rfc 3550 it's set to seq.
            stm_src->ssrc_stats.bad_seq = RTP_SEQ_MOD + 1;
            stm_src->ssrc_stats.cycles = 0;
            stm_src->ssrc_stats.received = 0;
            stm_src->ssrc_stats.received_prior = 0;
            stm_src->ssrc_stats.expected_prior = 1;
        }

        rtp_update_seq(stm_src, RTP_PKT_SEQ(pkt));

        if (!rtp_sess->ptdefs[pkt->pt]
            || !(rate = (rtp_sess->ptdefs[pkt->pt]->rate)))
            rate = RTP_DEF_CLK_RATE;

        transit = (uint32_t) (((double) now.tv_sec +
                   (double) now.tv_usec / 1000000.0) *
                   (double) rate) - ntohl(pkt->time);
        delta = transit - stm_src->ssrc_stats.transit;
        stm_src->ssrc_stats.transit = transit;

        if (stm_src->done_seek) {
            nms_printf(NMSML_NORM, "Seek reset performed on %u\n", stm_src->ssrc_stats.firstts);
            stm_src->done_seek = 0;
        }
        else {
            if (delta < 0)
                delta = -delta;
            stm_src->ssrc_stats.jitter +=
                (1. / 16.) * ((double) delta - stm_src->ssrc_stats.jitter);
        }
        break;
    case SSRC_NEW:
        rtp_sess->sess_stats.senders++;
        rtp_sess->sess_stats.members++;
    case SSRC_RTPNEW:
        (stm_src->ssrc_stats).probation = MIN_SEQUENTIAL;
        (stm_src->ssrc_stats).max_seq = RTP_PKT_SEQ(pkt) - 1;

        if (!rtp_sess->ptdefs[pkt->pt]
            || !(rate = (rtp_sess->ptdefs[pkt->pt]->rate)))
            rate = RTP_DEF_CLK_RATE;
        (stm_src->ssrc_stats).transit =
            (uint32_t) (((double) now.tv_sec +
                   (double) now.tv_usec / 1000000.0) *
                  (double) rate) - ntohl(pkt->time);
        (stm_src->ssrc_stats).jitter = 0;
        (stm_src->ssrc_stats).firstts = ntohl(pkt->time);
        (stm_src->ssrc_stats).firsttv = now;
        // rtp_init_seq(stm_src, RTP_PKT_SEQ(pkt));
        rtp_update_seq(stm_src, RTP_PKT_SEQ(pkt));
        break;
    case SSRC_COLLISION:
        bprmv(&(rtp_sess->bp), &(stm_src->po), slot);
        return 0;
        break;
    case -1:
        return 1;
        break;
    default:
        break;
    }

    switch (poadd(&(stm_src->po), slot, (stm_src->ssrc_stats).cycles)) {
    case PKT_DUPLICATED:
        nms_printf(NMSML_VERB,
               "WARNING: Duplicate pkt found... discarded\n");
        bpfree(&(rtp_sess->bp), slot);
        return 0;
        break;
    case PKT_MISORDERED:
        nms_printf(NMSML_VERB,
               "WARNING: Misordered pkt found... reordered\n");
        break;
    default:
        break;
    }

    ((stm_src->po).pobuff[slot]).pktlen = n;

    return 0;
}
