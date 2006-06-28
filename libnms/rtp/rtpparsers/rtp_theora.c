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
#include "rtp_xiph.h"




//get standard mkv/nut/ffmpeg configuration packet from an rtp one
static int cfg_fixup(rtp_theora_t *theo, rtp_frame *fr, rtp_pkt *pkt)
{
    unsigned char comment[] =
    /*quite minimal comment*/
    { 0x81,'t','h','e','o','r','a',
        10,0,0,0,
                't','h','e','o','r','a','-','r','t','p',
                0,0,0,0};

    int err = cfg_parse(theo, fr);
    if (err) return err;

    fr->len = fr->len + sizeof(comment);
    fr->len = realloc(fr->data, fr->len);

    memcpy(fr->len, fr->data, 30); // id packet
    memcpy(fr->len+30, comment, sizeof(comment)); // comment packet
    memcpy(fr->len+30+sizeof(comment), fr->data+30, fr->len-30);
    theo->id = RTP_VORB_ID(pkt);
    //cfg_cache_append() //XXX
    return 0;
}

static int single_parse(rtp_theora_t *theo, rtp_pkt *pkt, rtp_frame *fr,
                        rtp_ssrc *ssrc, int offset)
{

    int len = RTP_VORB_LEN(pkt,offset);

    if (len > fr->len){
        realloc(fr->buf, len);
        fr->len = len;
    }
    
    memcpy(fr->data, RTP_VORB_DATA(pkt, offset), fr->len);
    
    if( theo->cfg_id != RTP_VORB_ID(pkt) || //not the current id
//        !cfg_cache_find(theo,RTP_VORB_ID(pkt)); //XXX
        RTP_VORB_T(pkt) != 1                //not a config packet
    ) return RTP_PARSER_ERROR;
    
    theo->pkts--;
    rtp_rm_pkt(ssrc);

    if (RTP_VORB_T(pkt) == 1)
        return cfg_fixup(theo, fr);
    else
    {
        theo->curr_bs = ptk_blocksize(theo);
        if(theo->prev_bs)
            fr->timestamp += (theo->curr_bs + theo->prev_bs)/4;
        theo->prev_bs = theo->curr_bs;
    }

    return 0;
}

static int pack_parse(rtp_theora_t *theo, rtp_pkt *pkt, rtp_frame *fr,
                      rtp_ssrc *ssrc)
{
    single_parse(theo, pkt, ssrc, fr, theo->offset);
    theo->offset+=fr->len;
    return 0;
}

static int frag_parse(rtp_theora_t *theo, rtp_pkt *pkt, rtp_frame *fr,
                      rtp_ssrc *ssrc)
{
    int len;
    
    rtp_rm_pkt(ssrc);

    switch (RTP_VORB_T(pkt))
    {
    case 1:
        theo->len = 0;
    case 2:
        len = RTP_VORB_LEN(pkt,4);
        theo->buf = realloc(theo->buf, vorb->len + len);
        memcpy(theo->buf + theo->len, RTP_VORB_DATA(pkt, 4), len);
        theo->len+=len;
        return 0; //FIXME
    case 3:
        len = RTP_VORB_LEN(pkt,4);
        theo->buf = realloc(theo->buf, vorb->len + len);
        memcpy(theo->buf + theo->len, RTP_VORB_DATA(pkt, 4), len);
        theo->len+=len;
        
        if (theo->len > fr->len) {
            realloc(fr->buf, theo->len);
            fr->len = theo->len;
        }
        memcpy(fr->data, theo->buf, fr->len);

        if (RTP_VORB_T(pkt) == 1)
            return cfg_fixup(theo, fr);
        else
        {
            theo->curr_bs = ptk_blocksize(theo);
            if(theo->prev_bs)
                fr->timestamp += (theo->curr_bs + theo->prev_bs)/4;
            theo->prev_bs = theo->curr_bs;
        }
        return 0
    default: //      
        return -1;
    }

}


static int rtp_parse(rtp_ssrc *stm_src,
                     rtp_frame *fr, rtp_buff *config)
{
    rtp_pkt *pkt;
    int len;

    rtp_theora_t *theo = stm_src->prsr_privs[pt];

    //if I don't have previous work
    if (!theo->pkts)
    {
        //get a new packet
        if ( !(pkt=rtp_get_pkt(stm_src, &len)) )
		return RTP_BUFF_EMPTY;
        //get the number of packets stuffed in the rtp
        theo->pkts = RTP_VORB_PKTS(pkt);
    
        //some error checking
        if (theo->pkts >0 && (RTP_VORB_F(pkt) || !RTP_VORB_T(pkt)))
            return RTP_PARSE_ERROR;
        //single packet, easy case
        if (theo->pkts = 1)
            return single_parse(theo, pkt, fr, stm_src, 4);
        if (RTP_VORB_F(pkt))
            return frag_parse(theo, pkt, fr, stm_src);
        theo->offset = 4;
    }
    return pack_parse (theo, pkt, fr, stm_src);
}

