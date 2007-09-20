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

#include <nemesi/rtcp.h>

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
            timeval_subtract(&offset, &now,
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
