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
    int id;
    //timestamp related
    long int prev_bs;
    long int curr_bs;

    int modes;
    long blocksizes[2];
    int param_blockflag[64];

} rtp_vorbis_t

static rtpparser_info served = {
        -1,
        {"VORBIS", NULL}
};

RTPPRSR(vorbis);

//helpers
static int rtp_ilog(unsigned int v){
  int ret=0;
  while(v){
    ret++;
    v>>=1;
  }
  return(ret);
}

static int icount(unsigned int v){
  int ret=0;
  while(v){
    ret+=v&1;
    v>>=1;
  }
  return(ret);
}

long 
maptype_quantvals(int entries, int dim)
{
  long vals=floor(pow((float)entries,1.f/dim));

  /* the above *should* be reliable, but we'll not assume that FP is
     ever reliable when bitstream sync is at stake; verify via integer
     means that vals really is the greatest value of dim for which
     vals^b->bim <= b->entries */
  /* treat the above as an initial guess */
  while(1){
    long acc=1;
    long acc1=1;
    int i;
    for(i=0;i<dim;i++){
      acc*=vals;
      acc1*=vals+1;
    }
    if(acc<=entries && acc1>entries){
      return(vals);
    }else{
      if(acc>entries){
	vals--;
      }else{
	vals++;
      }
    }
  }
}


int cfg_parse(rtp_vorbis *vorb, rtp_frame *fr)
{
    bit_context opb;
    int num, i, j, k=-1, channels;
    
    //id packet
    vorb->blocksizes[0] = 1<<(fr->data[28]&0x0f);
    vorb->blocksizes[1] = 1<<(fr->data[28]>>4);
    channels = fr->data[11];
    //cb packet
    bit_readinit(&opb, fr->data + 30, fr->len - 30);
    if (5 != bit_read(&opb,8)) return RTP_PARSER_ERROR;
    
    for(i=6;i>0;i--) bit_read(&opb,8);

    num = bit_read(&opb,8)+1;
    
    for(;num>0;num--)
    {
            int entries, quantvals=-1, maptype, q_quant, dim;
            bit_read(&opb,24); //magic
            dim = bit_read(&opb,16); //dimensions
            entries = bit_read(&opb,24); //entries
            
            switch(bit_read(&opb,1)){
            case 0: //unordered
                    if(bit_read(&opb,1))
                    {
                            for(i=0; i<entries; i++)
                                    if(bit_read(&opb,1))
                                            bit_read(&opb,5);
                    }
                    else
                            for(i=0; i<entries; i++)
                                    bit_read(&opb,5);
                    break;
            case 1: //ordered
                    bit_read(&opb,5); //len
                    for(i=0;i<entries;)
                            i+=bit_read(&opb,rtp_ilog(entries-i));
                    break;
            }
            switch((maptype=bit_read(&opb,4))){
            case 0:
                    break;

            case 1: case 2:
                    bit_read(&opb,32);
                    bit_read(&opb,32);
                    q_quant = bit_read(&opb,4) + 1;
                    bit_read(&opb,1);
                    
                    switch (maptype){
                    case 1:
                            quantvals = maptype_quantvals(entries,dim);
                            break;
                    
                    case 2:
                            quantvals = entries*dim;
                            break;
                    }
                    
                    for(i=0;i<quantvals;i++)
                            bit_read(&opb,q_quant);
                    break;
            }
            
    }
    
    //times
    num = bit_read(&opb,6)+1;
    for(;num>0;num--)
            bit_read(&opb,16);
    
    //floors
    num = bit_read(&opb,6)+1;
    for(;num>0;num--)
            switch(bit_read(&opb,16)){
                    
            case 0:
                    bit_read(&opb,16);
                    bit_read(&opb,16);
                    bit_read(&opb,16);
                    bit_read(&opb,6);
                    bit_read(&opb,8);
                    for(i=bit_read(&opb,4)+1;i>0;i--)
                            bit_read(&opb,8);
                    break;
            case 1:
            {
                            int pclass[31], pdim[16],
                                rangebits, 
                                partitions=bit_read(&opb,5);
                    for(i=0;i<partitions;i++)
                    {
                            pclass[i]=bit_read(&opb,4);
                            if(k<pclass[i])k=pclass[i];
                    }
                    for(j=0;j<k+1;j++){
                            int subs;
                            pdim[j] = bit_read(&opb,3)+1; // dim
                            subs=bit_read(&opb,2);
                            if(subs) bit_read(&opb,8); // book
                            for(i=0;i<(1<<subs);i++)
                                    bit_read(&opb,8);
                    }

                    bit_read(&opb,2);
                    rangebits=bit_read(&opb,4);
                    for(j=0,k=0;j<partitions;j++)
                    {
                                    bit_read(&opb,rangebits*pdim[pclass[j]]);
                    }
            }
    }

    //residues
    num = bit_read(&opb,6)+1;
    for(;num>0;num--)
    {
            int partitions,acc=0;
            bit_read(&opb,16); // unpack is always the same
            bit_read(&opb,24); // begin
            bit_read(&opb,24); // end
            bit_read(&opb,24); // grouping
            partitions=bit_read(&opb,6)+1;
            bit_read(&opb,8); // groupbook
            for(j=0;j<partitions;j++){
                    int cascade=bit_read(&opb,3);
                    if(bit_read(&opb,1))
                            cascade|=(bit_read(&opb,5)<<3);
                    acc+=icount(cascade);
            }
            for(j=0;j<acc;j++)
                    bit_read(&opb,8);
    }
    //maps
    num = bit_read(&opb,6)+1;  
    for(;num>0;num--)
    {
            int submaps=1;
            bit_read(&opb,16); // maps is just mapping0
            if(bit_read(&opb,1))
                    submaps=bit_read(&opb,4)+1;
            if(bit_read(&opb,1))
                    for(i=bit_read(&opb,8)+1;i>0;i--){
                            bit_read(&opb,rtp_ilog(channels-1));
                            bit_read(&opb,rtp_ilog(channels-1));
                    }
            bit_read(&opb,2);
            if(submaps>1)
                    for(i=0;i<channels;i++)
                            bit_read(&opb,4);
            for(i=0;i<submaps;i++){
                    bit_read(&opb,8);
                    bit_read(&opb,8);
                    bit_read(&opb,8);
            }
    }

    //modes
    vorb->modes = bit_read(&opb,6)+1;
    for(i=0;i<vorb->modes;i++)
    {
            vorb->param_blockflag[i] = bit_read(&opb,1); //blockflag
            bit_read(&opb,16);
            bit_read(&opb,16);
            bit_read(&opb,8);
    }

    return 0; //FIXME add some checks and return -1 on failure
}



//get the blocksize from a full data packet
static long pkt_blocksize(rtp_vorbis_t *vorb, rtp_packet *fr)
{
    int mode;
    bit_context bc = bit_init(fr->data, fr->len);    
    read_bit(bc,1);

    mode = bit_read(&bc,rtp_ilog(vorb->modes));
    
    return vorb->blocksizes[vorb->param_blockflag[mode]];
}

//get standard mkv/nut/ffmpeg configuration packet from an rtp one
static int cfg_fixup(rtp_vorbis_t *vorb, rtp_frame *fr, rtp_pkt *pkt)
{
    unsigned char comment[] = 
    /*quite minimal comment*/
    { 3,'v','o','r','b','i','s', 
        10,0,0,0, 
                'v','o','r','b','i','s','-','r','t','p',
                0,0,0,0, 
                1};

    int err = cfg_parse(vorb, fr);
    if (err) return err;

    fr->len = fr->len + sizeof(comment);
    fr->len = realloc(fr->data, fr->len);

    memcpy(fr->len, fr->data, 30); // id packet
    memcpy(fr->len+30, comment, sizeof(comment)); // comment packet
    memcpy(fr->len+30+sizeof(comment), fr->data+30, fr->len-30);
    vorb->id = RTP_VORB_ID(pkt);
    //cfg_cache_append() //XXX
    return 0;
}

static int single_parse(rtp_vorbis_t *vorb, rtp_pkt *pkt, rtp_frame *fr,
                        struct rtp_ssrc *ssrc, int offset)
{

    int len = RTP_VORB_LEN(pkt,offset);

    if (len > fr->len){
        realloc(fr->buf, len);
        fr->len = len;
    }
    
    memcpy(fr->data, RTP_VORB_DATA(pkt, offset), fr->len);
    
    if( vorb->cfg_id != RTP_VORB_ID(pkt) || //not the current id
//        !cfg_cache_find(vorb,RTP_VORB_ID(pkt)); //XXX
        RTP_VORB_T(pkt) != 1                //not a config packet
    ) return RTP_PARSER_ERROR;
    
    vorb->pkts--;
    rtp_rm_pkt(sess, ssrc);

    if (RTP_VORB_T(pkt) == 1)
        return cfg_fixup(vorb, fr);
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

