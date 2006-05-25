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


#define RTP_VORB_ID(pkt)    (   RTP_PKT_DATA(pkt)[0]<<16 +\
                                RTP_PKT_DATA(pkt)[1]<<8 +\
                                RTP_PKT_DATA(pkt)[2]    )
#define RTP_VORB_F(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0xc0)>> 6)
#define RTP_VORB_T(pkt)     ((RTP_PKT_DATA(pkt)[3]& 0x30)>> 4)
#define RTP_VORB_PKTS(pkt)  (RTP_PKT_DATA(pkt)[3]& 0x0F)
#define RTP_VORB_LEN(pkt,off)   (RTP_PKT_DATA(pkt)[off]<<8+\
                                 RTP_PKT_DATA(pkt)[off+1])
#define RTP_VORB_DATA(pkt,off)  (RTP_PKT_DATA(pkt)[off+2])

typedef struct vorbis_s {
    long offset;
    long len;
    int pkts;
    char *buf;
    long timestamp;
} vorbis_t

static rtpparser_info served = {
        -1,
        {"VORBIS", NULL}
};

RTPPRSR(vorbis);

//helpers

//get the blocksize from a full data packet
static long pkt_blocksize(rtp_vorbis_t *vorb);

//get standard mkv/nut/ffmpeg configuration packet from an rtp one
static int cfg_fixup(char *buff, size_t len);

static rtp_frame *single_parse(rtp_vorbis_t *vorb, rtp_pkt *pkt,
                               rtp_frame fr, struct rtp_ssrc *ssrc, int offset)
{

    int len = RTP_VORB_LEN(pkt,offset);

    if (len > fr->len){
        realloc(fr->buf, len);
        fr->len = len;
    }
    
    memcpy(fr->data, RTP_VORB_DATA(pkt, offset), fr->len);
    
    vorb->pkts--;
    rtp_rm_pkt(sess, ssrc);

    if (RTP_VORB_T(pkt) == 1)
        return cfg_fixup(vorb);
    else
    {
        vorb->curr_bs = ptk_blocksize(vorb);
        if(vorb->prev_bs)
            fr->timestamp += (vorb->curr_bs + vorb->prev_bs)/4;
        vorb->prev_bs = vorb->curr_bs;
    }
   
    return 0;
}

static int pack_parse(rtp_vorbis_t *vorb, rtp_pkt *pkt, rtp_frame *fr,
                      struct rtp_ssrc *ssrc)
{
    single_parse(vorb, pkt, ssrc, fr, vorb->offset);
    vorb->offset+=fr->len;
    return 0;
}

static int frag_parse(rtp_vorbis_t *vorb, rtp_pkt *pkt, rtp_frame *fr,
                      struct rtp_ssrc *ssrc)
{
    int len;
    
    rtp_rm_pkt(sess, ssrc);

    switch (RTP_VORB_T(pkt))
    {
    case 1:
        vorb->len = 0;
    case 2:
        len = RTP_VORB_LEN(pkt,4);
        vorb->buf = realloc(vorb->buf, vorb->len + len);
        memcpy(vorb->buf + vorb->len, RTP_VORB_DATA(pkt, 4), len);
        vorb->len+=len;
        return 0; //FIXME
    case 3:
        len = RTP_VORB_LEN(pkt,4);
        vorb->buf = realloc(vorb->buf, vorb->len + len);
        memcpy(vorb->buf + vorb->len, RTP_VORB_DATA(pkt, 4), len);
        vorb->len+=len;
        
        if (vorb->len > fr->len) {
            realloc(fr->buf, vorb->len);
            fr->len = vorb->len;
        }
        memcpy(fr->data, vorb->buf, fr->len);

        if (RTP_VORB_T(pkt) == 1)
            return cfg_fixup(vorb, fr);
        else
        {
            vorb->curr_bs = ptk_blocksize(vorb);
            if(vorb->prev_bs)
                fr->timestamp += (vorb->curr_bs + vorb->prev_bs)/4;
            vorb->prev_bs = vorb->curr_bs;
        }
        return 0
    default: //      
        return -1;
    }

}


static int rtp_parse(rtp_fnc_type prsr_type, struct rtp_ssrc *stm_src,
                     rtp_frame *fr)
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
        //get the number of packets stuffed in the rtp
        vorb->pkts = RTP_VORB_PKTS(pkt);
    
        //some error checking
        if (vorb->pkts >0 && (RTP_VORB_F(pkt) || !RTP_VORB_T(pkt)))
            return RTP_PARSE_ERROR;
        //single packet, easy case
        if (vorb->pkts = 1)
            return single_parse(vorb, pkt, fr, stm_src, 4);
        if (RTP_VORB_F(pkt))
            return frag_parse(vorb, pkt, fr, stm_src);
        vorb->offset = 4;
    }
    return pack_parse (vorb, pkt, fr, stm_src);
}

