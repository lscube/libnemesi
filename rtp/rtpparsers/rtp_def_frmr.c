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

int rtp_def_frmr(struct rtp_session *rtp_sess, struct rtp_ssrc *stm_src, char *dst, size_t dst_size, uint32 *timestamp)
{
    rtp_pkt *pkt;
    size_t pkt_len, dst_used=0;
    size_t to_cpy;
    
    pkt=rtp_get_pkt(stm_src, (int *)&pkt_len);
    *timestamp = RTP_PKT_TS(pkt);
    
    do {
        to_cpy = min(pkt_len, dst_size);
        memcpy(dst, RTP_PKT_DATA(pkt), to_cpy);
        dst_used += to_cpy;
        rtp_rm_pkt(rtp_sess, stm_src);
    } while ( (dst_used<dst_size) && (pkt=rtp_get_pkt_nonblock(stm_src, (int *)&pkt_len)) && (RTP_PKT_TS(pkt)==*timestamp) );
    
    return dst_used;
}

int rtp_def_frmr_nonblock(struct rtp_session *rtp_sess, struct rtp_ssrc *stm_src, char *dst, size_t dst_size, uint32 *timestamp)
{    
    rtp_pkt *pkt;
    size_t pkt_len, dst_used=0;
    size_t to_cpy;
    
    if ( !(pkt=rtp_get_pkt_nonblock(stm_src, (int *)&pkt_len)) )
        return RTP_FILL_EMPTY;
    *timestamp = RTP_PKT_TS(pkt);
    
    do {
        to_cpy = min(pkt_len, dst_size);
        memcpy(dst, RTP_PKT_DATA(pkt), to_cpy);
        dst_used += to_cpy;
        rtp_rm_pkt(rtp_sess, stm_src);
    } while ( (dst_used<dst_size) && (pkt=rtp_get_pkt_nonblock(stm_src, (int *)&pkt_len)) && (RTP_PKT_TS(pkt)==*timestamp) );
    
    return dst_used;
}

