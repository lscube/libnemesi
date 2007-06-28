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
#include <nemesi/rtpptdefs.h>

double rtp_get_next_ts(rtp_ssrc * stm_src)
{                // TODO: calculate time using RTCP infos
    rtp_pkt *pkt;

    if (!(pkt = rtp_get_pkt(stm_src, NULL)))
        return -1;

    return ((double) (RTP_PKT_TS(pkt) - stm_src->ssrc_stats.firstts)) /
        (double) stm_src->rtp_sess->ptdefs[pkt->pt]->rate;
}
