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

void rtp_update_seq(rtp_ssrc * stm_src, uint16 seq)
{
    struct rtp_ssrc_stats *stats = &(stm_src->ssrc_stats);
    uint16 udelta = seq - stats->max_seq;

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
