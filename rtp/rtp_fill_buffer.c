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

    /* If we did a seek, we must wait for seek reset and bufferpool clean up,
     * so wait until rtp_recv receives the first new packet and resets the bufferpool
     */
    if (stm_src->done_seek) {
        usleep(0);
        return RTP_BUFF_EMPTY;
    }

    if (!(pkt = rtp_get_pkt(stm_src, NULL))) {
        usleep(0);
        return RTP_BUFF_EMPTY;
    }

    fr->pt = RTP_PKT_PT(pkt);
    fr->timestamp = RTP_PKT_TS(pkt);

    fr->time_sec =
        ((double) (fr->timestamp - stm_src->ssrc_stats.firstts)) /
        (double) stm_src->rtp_sess->ptdefs[pkt->pt]->rate;

    if (fr->time_sec > 1000) {
        fprintf(stderr, "Out of sync timestamp: %ld - %u\n", fr->timestamp, stm_src->ssrc_stats.firstts);
        rtp_rm_pkt(stm_src);
        return RTP_BUFF_EMPTY;
    }

    fr->fps = stm_src->rtp_sess->ptdefs[fr->pt]->fps = 
        (double) stm_src->rtp_sess->ptdefs[pkt->pt]->rate/
            abs(fr->timestamp - stm_src->ssrc_stats.lastts);
    stm_src->rtp_sess->ptdefs[fr->pt]->prev_timestamp =
        stm_src->ssrc_stats.lastts = fr->timestamp;

    while ((err = stm_src->rtp_sess->parsers[fr->pt] (stm_src, fr, config)) 
            == EAGAIN);
    return err;
}
