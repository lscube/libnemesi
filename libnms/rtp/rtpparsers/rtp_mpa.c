/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

static rtpparser_info served = {
	14,
	{"MPA", NULL}
};

RTPPRSR(mpa);

#define DEFAULT_MPA_DATA_FRAME 1024

#define MPA_IS_SYNC(buff_data) ((buff_data[0]==0xff) && ((buff_data[1] & 0xe0)==0xe0))

// #define MPA_RTPSUBHDR 4

#define BITRATEMATRIX { \
	{0,     0,     0,     0,     0     }, \
	{32000, 32000, 32000, 32000, 8000  }, \
	{64000, 48000, 40000, 48000, 16000 }, \
	{96000, 56000, 48000, 56000, 24000 }, \
	{128000,64000, 56000, 64000, 32000 }, \
	{160000,80000, 64000, 80000, 40000 }, \
	{192000,96000, 80000, 96000, 48000 }, \
	{224000,112000,96000, 112000,56000 }, \
	{256000,128000,112000,128000,64000 }, \
	{288000,160000,128000,144000,80000 }, \
	{320000,192000,160000,160000,96000 }, \
	{352000,224000,192000,176000,112000}, \
	{384000,256000,224000,192000,128000}, \
	{416000,320000,256000,224000,144000}, \
	{448000,384000,320000,256000,160000}, \
	{0,     0,     0,     0,     0     } \
}

#define SRATEMATRIX { \
	{44100,	22050,	11025	}, \
	{48000,	24000,	12000	}, \
	{32000,	16000,	8000	}, \
	{-1,	-1,	-1	} \
}

typedef struct {
	char *data;
	uint32 data_size;
} rtp_mpa;

typedef struct {
	uint32 mbz:16;
	uint32 frag_offset:16;
} rtp_mpa_hdr;

#define RTP_MPA_HDR(x) ((rtp_mpa_hdr *)x)

//static int rtp_parse(rtp_fnc_type prsr_type, struct rtp_session *rtp_sess, struct rtp_ssrc *stm_src, char *dst, size_t dst_size, uint32 *timestamp)
//static rtp_parser rtp_parse
static int rtp_parse(struct rtp_ssrc *stm_src, unsigned pt, rtp_frame *fr)
{
	// XXX tmp vars to be removed
	rtp_fnc_type prsr_type = rtp_n_blk;
	struct rtp_session *rtp_sess=stm_src->rtp_sess;
	char dst[65535];
	size_t dst_size = sizeof(dst);
	uint32 *timestamp;
	// end of tmp vars
	rtp_mpa *mpa_priv = stm_src->prsr_privs[pt];
	rtp_pkt *pkt;
	size_t pkt_len; //, dst_used=0;
	size_t to_cpy;
	rtp_mpa_hdr *rtp_subhdr;
	
	// XXX should we check if payload/mime-type are compatible with parser?
	  
	if ( !(pkt=rtp_get_pkt(prsr_type, stm_src, (int *)&pkt_len)) )
		return RTP_BUFF_EMPTY; // valid only for NON blocking version.
	
	*timestamp = RTP_PKT_TS(pkt);
	
	// init private struct if this is the first time we're called
	if (!mpa_priv) {
		if ( !(mpa_priv=malloc(sizeof(rtp_mpa))) )
			return RTP_ERRALLOC;
		if ( !(mpa_priv->data=malloc(DEFAULT_MPA_DATA_FRAME)) )
			return RTP_ERRALLOC;
		mpa_priv->data_size = DEFAULT_MPA_DATA_FRAME;
		
		// save new private struct
		stm_src->prsr_privs[pt] = mpa_priv;
	}
	
	// discard pkt if it's fragmented and the first fragment was lost
	while (RTP_MPA_HDR(RTP_PKT_DATA(pkt))->frag_offset) {
		rtp_rm_pkt(rtp_sess, stm_src);
		if ( !(pkt=rtp_get_pkt(prsr_type, stm_src, (int *)&pkt_len)) )
			return RTP_BUFF_EMPTY; 
	}
	// TODO: support for fragmented mpa frames
	to_cpy = min(pkt_len, dst_size);
	memcpy(dst, RTP_PKT_DATA(pkt)+4, to_cpy);
	rtp_rm_pkt(rtp_sess, stm_src);
	if ( to_cpy < pkt_len)
		return RTP_DST_TOO_SMALL;
	else
		return pkt_len;
#if 0
	do {
		to_cpy = min(pkt_len, dst_size);
		memcpy(dst, RTP_PKT_DATA(pkt)+4, to_cpy);
		dst_used += to_cpy;
		rtp_rm_pkt(rtp_sess, stm_src);
	} while ( (dst_used<dst_size) && (pkt=rtp_get_pkt(rtp_n_blk, stm_src, (int *)&pkt_len)) && (RTP_PKT_TS(pkt)==*timestamp) );
	
	return dst_used;
#endif
}

// private functions
#if 0
// static int mpa_sync(uint32 *header, InputStream *i_stream, mpa_data *mpa)
static int mpa_sync(uint32 *header, mpa_input *in, mpa_data *mpa)
{
	uint8 *sync_w = (uint8 *)header;
	int ret;

	// if ( (ret=istream_read(4, sync_w, i_stream)) != 4 )
	if ( (ret=mpa_read(4, sync_w, in)) != 4 )
		return (ret<0) ? ERR_PARSE : ERR_EOF;

	if (!mpa->probed) {
		/*look if ID3 tag is present*/
		if (!memcmp(sync_w, "ID3", 3)) { // ID3 tag present
			id3v2_hdr id3hdr;

			// fnc_log(FNC_LOG_DEBUG, "ID3v2 tag present in %s\n", i_stream->name);

			memcpy(&id3hdr, sync_w, 4);
			// if ( (ret = istream_read(ID3v2_HDRLEN - 4, &id3hdr.rev, i_stream)) != ID3v2_HDRLEN - 4 )
			if ( (ret = mpa_read(ID3v2_HDRLEN - 4, &id3hdr.rev, in)) != ID3v2_HDRLEN - 4 )
				return (ret<0) ? ERR_PARSE : ERR_EOF;
			// if ( (ret=mpa_read_id3v2(&id3hdr, i_stream, mpa)) )
			if ( (ret=mpa_read_id3v2(&id3hdr, in, mpa)) )
				return ret;
			// if ( (ret=istream_read(4, sync_w, i_stream)) != 4 )
			if ( (ret=mpa_read(4, sync_w, in)) != 4 )
				return (ret<0) ? ERR_PARSE : ERR_EOF;
		}
	}
	while ( !MPA_IS_SYNC(sync_w) ) {
		*header >>= 8;
		// sync_w[0]=sync_w[1];
		// sync_w[1]=sync_w[2];
		// sync_w[2]=sync_w[3];

		// if ( (ret=istream_read(1, &sync_w[3], i_stream)) != 1 )
		if ( (ret=mpa_read(1, &sync_w[3], in)) != 1 )
			return (ret<0) ? ERR_PARSE : ERR_EOF;
		//fnc_log(FNC_LOG_DEBUG, "[MPA] sync: %X%X%X%X\n", sync_w[0], sync_w[1], sync_w[2], sync_w[3]);
	}

	return 0;
}

static int mpa_decode_header(uint32 header, MediaProperties *properties, mpa_data *mpa)
{
	int RowIndex,ColIndex;
	uint8 *buff_data = (uint8 * )&header;
	int padding;
	int BitrateMatrix[16][5] = BITRATEMATRIX;
	float SRateMatrix[4][3] = SRATEMATRIX;
	enum {MPA_MPEG_2_5=0, MPA_MPEG_RES, MPA_MPEG_2, MPA_MPEG_1} id;
	enum {MPA_LAYER_RES=0, MPA_LAYER_III, MPA_LAYER_II, MPA_LAYER_I} layer;
	// uint32 frame_size;
	uint32 bit_rate;
	float sample_rate;/*SamplingFrequency*/
	int pkt_len;

	if ( !MPA_IS_SYNC(buff_data) ) return -1; /*syncword not found*/

	id = (buff_data[1] & 0x18) >> 3;
	layer = (buff_data[1] & 0x06) >> 1;

	switch (id<< 2 | layer) {
		case MPA_MPEG_1<<2|MPA_LAYER_I:		// MPEG 1 layer I
			ColIndex = 0;
			break;
		case MPA_MPEG_1<<2|MPA_LAYER_II:	// MPEG 1 layer II
			ColIndex = 1;
			break;
		case MPA_MPEG_1<<2|MPA_LAYER_III:	// MPEG 1 layer III
			ColIndex = 2;
			break;
		case MPA_MPEG_2<<2|MPA_LAYER_I:		// MPEG 2 layer I
		case MPA_MPEG_2_5<<2|MPA_LAYER_I:	// MPEG 2.5 layer I
			ColIndex = 3;
			break;
		case MPA_MPEG_2<<2|MPA_LAYER_II:	// MPEG 2 layer II
		case MPA_MPEG_2<<2|MPA_LAYER_III:	// MPEG 2 layer III
		case MPA_MPEG_2_5<<2|MPA_LAYER_II:	// MPEG 2.5 layer II
		case MPA_MPEG_2_5<<2|MPA_LAYER_III:	// MPEG 2.5 layer III
			ColIndex = 4;
			break;
		default: return -1; break;
	}
	
	RowIndex = (buff_data[2] & 0xf0) >> 4;
	if (RowIndex == 0xF) // bad bitrate
		return -1;
	if (RowIndex == 0) // free bitrate: not supported
		return -1;
	bit_rate = BitrateMatrix[RowIndex][ColIndex];

	switch (id) {
		case MPA_MPEG_1: ColIndex = 0; break;
		case MPA_MPEG_2: ColIndex = 1; break;
		case MPA_MPEG_2_5: ColIndex = 2; break;
		default:
			return -1;
			break;
	}

	RowIndex = (buff_data[2] & 0x0c) >> 2;
	if (RowIndex == 3) // reserved
		return -1;
	sample_rate = SRateMatrix[RowIndex][ColIndex];

	// padding
	padding = (buff_data[2] & 0x02) >> 1;

	// packet len
	if (layer == MPA_LAYER_I) { // layer 1
		// frame_size = 384;
		pkt_len=((12 * bit_rate)/sample_rate + padding)* 4;
	} else { // layer 2 or 3
		// frame_size = 1152;
		pkt_len= 144 * bit_rate /sample_rate + padding;
	}
	
#if DEBUG
	mpa_info(mpa, properties);
#endif // DEBUG
	
	return 0;// count; /*return 4*/
}
#endif