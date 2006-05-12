/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *  	
 *	Luca Barbato - lu_zero@gentoo.org
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include "rtpparser.h"

//vorbis header
#define RTP_VORB_ID(pkt)    (   RTP_PKT_DATA(pkt)[0]<<16 +\
                                RTP_PKT_DATA(pkt)[1]<<8 +\
                                RTP_PKT_DATA(pkt)[2]    )
#define RTP_VORB_F(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0xc0)>> 6)
#define RTP_VORB_T(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0x30)>> 4)
#define RTP_VORB_PKTS(pkt)  (RTP_PKT_DATA(pkt)[3]& 0x0F)
#define RTP_VORB_LEN(pkt,off)   (RTP_PKT_DATA(pkt)[off]<<8+\
                                 RTP_PKT_DATA(pkt)[off+1])


static rtpparser_info served = {
        -1,
        {"VORBIS", NULL}
};

RTPPRSR(vorbis);

//helpers

//get the blocksize from a full data packet
long pkt_blocksize(rtp_vorbis_t *vorb);

//get standard mkv/nut/ffmpeg configuration packet from an rtp one
int cfg_fixup(char *, size_t len);

static int rtp_parse(rtp_fnc_type prsr_type, struct rtp_session *rtp_sess, struct rtp_ssrc *stm_src, char *dst, size_t dst_size, uint32 *timestamp)
{
    rtp_pkt *pkt;
    int len, pt = 96; //FIXME I need to know my pt!!!

    rtp_vorbis_t *vorb = stm_src->prsr_privs[pt];

    //if I don't have previous work
    if (!vorb->pkts)
    {
        //get a new packet
        if ( !(pkt=rtp_get_pkt(prsr_type, stm_src, &len)) )
		return RTP_BUFF_EMPTY;
        //get the pkts in the rtp
        vorb->pkts = RTP_VORB_PKTS(pkt);
    
        //if they are more than 0 and is a frag or non raw data
        if (vorb->pkts >0 && (RTP_VORB_F(pkt) || !RTP_VORB_T(pkt)))
            return RTP_PARSE_ERROR;
        //single packet, easy case
        if (vorb->pkts = 1 && !RTP_VORB_F(pkt))
            return single_parse(pkt, dst, dst_size, rtp_sess, stm_src);
        if (RTP_VORB_F(pkt))
            return frag_parse(pkt, dst, dst_size, vorb, );
    }
    return pack_parse (pkt, dst, dst_size, vorb, rtp_sess, stm_src);       
}

