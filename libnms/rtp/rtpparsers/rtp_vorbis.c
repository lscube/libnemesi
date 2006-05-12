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

    //get a new packet
    if(! (pkt = rtp_get_pkt( prsr_type, stm_src, &len )) )
        return RTP_BUFF_EMPTY;
    if(dst_size<vorb->length+len)
        return RTP_DST_TOO_SMALL;

    while (!vorb->pkts) //FIXME 
    {        
        //vorbis packets in the rtp
        vorb->pkts = RTP_VORB_PKTS(pkt);

        switch(RTP_VORB_F(pkt))
        {
            case 0: //no frag
                vorb->offset = 4;
                vorb->length = 0;
                break;
            case 1: //start
                vorb->length = 0;
                vorb->packet = NULL;
            case 2: //cont
                vorb->bytes = RTP_VORB_LEN(pkt,4);
                memcpy(dst + vorb->length, RTP_PKT_DATA(pkt)+6, vorb->bytes);
                vorb->length += len;
                rtp_rm_pkt(rtp_sess, stm_src);
                continue;
            case 3: //end
                vorb->offset = 4;
                vorb->pkts=1;
                break;
            default: //we got a problem
                return RTP_PARSE_ERROR; //FIXME
        }

        //get a new packet
        if(! (pkt = rtp_get_pkt( prsr_type, stm_src, &len )) )
            return RTP_BUFF_EMPTY;
        if(dst_size<vorb->length+len)
            return RTP_DST_TOO_SMALL;

    }

    vorb->bytes = RTP_VORB_LEN(pkt,vorb->offset);
    vorb->offset+=2
    memcpy(dst + vorb->length, RTP_PKT_DATA(pkt)+vorb->offset, vorb->bytes);
    vorb->offset+= vorb->bytes;
    vorb->bytes += vorb->length;

    switch(RTP_VORB_T(pkt))
    {
        case 0: //raw data
            vorb->curr_bs = pkt_blocksize(vorb);
            if (vorb->prev_bs)
                vorb->timestamp += (vorb->curr_bs + vorb->prev_bs)/4;
            timestamp = vorb->timestamp;


        case 1: //configuration
                
        default: //ignore the rest
    }

    if (pkts == 0) rtp_rm_pkt(rtp_sess, stm_src);

}

