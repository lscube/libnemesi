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

/** @file rtcp_report.c
 * This file contains the functions that perform RTCP Receiver
 * and Sender Reports building and parsing.
 */

#include "rtcp.h"
#include "comm.h"
#include "utils.h"

/**
 * Builds the Receiver Report packet
 * @param rtp_sess The session for which to build the report
 * @param pkt The packet where to write the report
 * @return The Length of the generated report (number of 32bit words)
 */
int rtcp_build_rr(rtp_session * rtp_sess, rtcp_pkt * pkt)
{
    struct timeval now, offset;
    uint32_t linear;
    rtp_ssrc *stm_src;
    rtcp_rr_t *rr;
    uint32_t expected, expected_interval, received_interval, lost_interval;
    int32_t lost;

    rr = pkt->r.rr.rr;
    pkt->common.len = 0;

    for (stm_src = rtp_sess->ssrc_queue; stm_src; stm_src = stm_src->next) {
        if ((pkt->common.len * 4 + 2) > (MAX_PKT_SIZE - 4 * 6))
            /* No space left in UDP pkt */
            break;
        if (stm_src->ssrc_stats.received_prior !=
            stm_src->ssrc_stats.received) {
            pkt->common.count++;
            rr->ssrc = htonl(stm_src->ssrc);

            expected =
                stm_src->ssrc_stats.cycles +
                stm_src->ssrc_stats.max_seq -
                stm_src->ssrc_stats.base_seq + 1;
            expected_interval =
                expected - stm_src->ssrc_stats.expected_prior;
            stm_src->ssrc_stats.expected_prior = expected;
            received_interval =
                stm_src->ssrc_stats.received -
                stm_src->ssrc_stats.received_prior;
            stm_src->ssrc_stats.received_prior =
                stm_src->ssrc_stats.received;
            lost_interval = expected_interval - received_interval;

            if ((expected_interval == 0) || (lost_interval <= 0))
                rr->fraction = 0;
            else
                rr->fraction =
                    (uint8_t) ((lost_interval << 8) /
                         expected_interval);

            lost = min((int32_t)
                   (expected - stm_src->ssrc_stats.received -
                    1), 0x7fffff);
            lost = max(lost, -(1 << 23));
            rr->lost = ntohl24(lost);

            rr->last_seq =
                htonl(stm_src->ssrc_stats.cycles +
                  stm_src->ssrc_stats.max_seq);
            rr->jitter =
                htonl((uint32_t) stm_src->ssrc_stats.jitter);
            rr->last_sr =
                htonl(((stm_src->ssrc_stats.
                    ntplastsr[0] & 0x0000ffff) << 16) |
                  ((stm_src->ssrc_stats.
                    ntplastsr[1] & 0xffff0000)
                   >> 16));

            gettimeofday(&now, NULL);
            nms_timeval_subtract(&offset, &now,
                     &(stm_src->ssrc_stats.lastsr));
            linear =
                (offset.tv_sec +
                 (float) offset.tv_usec / 1000000) * (1 << 16);
            rr->dlsr =
                ((stm_src->ssrc_stats.lastsr.tv_sec !=
                  0) ? htonl(linear) : 0);

            rr++;
        }
    }

    pkt->common.ver = RTP_VERSION;
    pkt->common.pad = 0;
    pkt->common.pt = RTCP_RR;
    pkt->common.len = htons(pkt->common.count * 6 + 1);
    pkt->r.rr.ssrc = htonl(rtp_sess->local_ssrc);

    return (pkt->common.count * 6 + 2);
}

/**
 * Actually sends the Receiver Report for the given session
 * @param rtp_sess The Session for which to generate and send
 *                 the report packet
 */
int rtcp_send_rr(rtp_session * rtp_sess)
{
    rtcp_pkt *pkt;
    int len;
    uint32_t rr_buff[MAX_PKT_SIZE];
    rtp_ssrc *stm_src;

    memset(rr_buff, 0, MAX_PKT_SIZE * sizeof(uint32_t));
    pkt = (rtcp_pkt *) rr_buff;

    len = rtcp_build_rr(rtp_sess, pkt);    /* in 32 bit words */
    pkt = (rtcp_pkt *) (rr_buff + len);
    len += rtcp_build_sdes(rtp_sess, pkt, (MAX_PKT_SIZE >> 2) - len);    /* in 32 bit words */

    for (stm_src = rtp_sess->ssrc_queue; stm_src; stm_src = stm_src->next)
        if ( !(stm_src->no_rtcp) && stm_src->rtp_sess->transport.RTCP.sock.fd > 0) {
            switch(stm_src->rtp_sess->transport.type) {
            case UDP:
                if (sendto(stm_src->rtp_sess->transport.RTCP.sock.fd,
                       rr_buff, (len << 2), 0, stm_src->rtcp_from.addr,
                       stm_src->rtcp_from.addr_len) < 0)
                    nms_printf(NMSML_WARN,
                           "WARNING! Error while sending UDP RTCP pkt\n");
                else
                    nms_printf(NMSML_DBG1,
                           "RTCP RR packet sent\n");
                break;
            case SCTP:
            case TCP:
                if (send(stm_src->rtp_sess->transport.RTCP.sock.fd,
                       rr_buff, (len << 2), 0) < 0)
                    nms_printf(NMSML_WARN,
                           "WARNING! Error while sending local RTCP pkt\n");
                else
                    nms_printf(NMSML_DBG1,
                           "RTCP RR packet sent\n");
                break;
            default:
                nms_printf(NMSML_WARN, "Unsupported transport type on send_rr\n");
                break;
            }
        }

    return len;
}

/**
 * Receiver Report packet handling, actually does nothing
 */
int rtcp_parse_rr(rtcp_pkt * pkt)
{
    // TODO: handle rr packet
    nms_printf(NMSML_DBG1, "Received RR from SSRC: %u\n", pkt->r.rr.ssrc);
    return 0;
}

/**
 * Sender Report packet handling. Sets the NTP timestamp in the
 * the ssrc_stats of the given RTP_SSRC.
 * @param stm_src The SSRC for which the packet was received
 * @param pkt The packet itself
 * @return 0
 */
int rtcp_parse_sr(rtp_ssrc * stm_src, rtcp_pkt * pkt)
{
    nms_printf(NMSML_DBG1, "Received SR from SSRC: %u\n", pkt->r.sr.ssrc);
    gettimeofday(&(stm_src->ssrc_stats.lastsr), NULL);
    stm_src->ssrc_stats.ntplastsr[0] = ntohl(pkt->r.sr.si.ntp_seq);
    stm_src->ssrc_stats.ntplastsr[1] = ntohl(pkt->r.sr.si.ntp_frac);
    /* Per ora, non ci interessa altro. */
    /* Forse le altre informazioni possono */
    /* servire per un monitor RTP/RTCP */
    return 0;
}
