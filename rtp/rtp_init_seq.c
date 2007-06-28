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

void rtp_init_seq(rtp_ssrc * stm_src, uint16 seq)
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
