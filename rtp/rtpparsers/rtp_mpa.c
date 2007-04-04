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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "rtpparser.h"

static rtpparser_info served = {
	14,
	{"MPA", NULL}
};

RTP_PARSER(mpa);

#define DEFAULT_MPA_DATA_FRAME 1024

#define MPA_IS_SYNC(buff_data) (((buff_data)[0]==0xff) && (((buff_data)[1] & 0xe0)==0xe0))

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
	enum { MPA_MPEG_2_5 = 0, MPA_MPEG_RES, MPA_MPEG_2, MPA_MPEG_1 } id;
	enum { MPA_LAYER_RES = 0, MPA_LAYER_III, MPA_LAYER_II, MPA_LAYER_I
	} layer;
	uint32 bit_rate;
	float sample_rate;	/*SamplingFrequency */
	uint32 frame_size;
	uint32 frm_len;
	// *** probably not needed ***
#if 0
	uint32 probed;
	double time;
	// fragmentation suuport:
	uint8 fragmented;
	uint8 *frag_src;
	uint32 frag_src_nbytes;
	uint32 frag_offset;
#endif
} mpa_frm;

typedef struct {
//      mpa_data mpa_info;
	char *data;
	uint32 data_size;
} rtp_mpa;

typedef struct {
	uint32 mbz:16;
	uint32 frag_offset:16;
	uint8 data[1];
} rtp_mpa_pkt;

#define RTP_MPA_PKT(pkt)		((rtp_mpa_pkt *)(RTP_PKT_DATA(pkt)))
#define RTP_MPA_DATA_LEN(pkt, pkt_size)	((pkt && pkt_size) ? (RTP_PAYLOAD_SIZE(pkt, pkt_size)-4) : 0)
#define RTP_MPA_FRAG_OFFSET(pkt)	ntohs(RTP_MPA_PKT(pkt)->frag_offset)

// private functions
static int mpa_sync(uint8 **, size_t * /*, mpa_frm * */ );
static int mpa_decode_header(uint8 *, mpa_frm *);
#ifdef ENABLE_DEBUG
// debug function to diplay MPA header information
static void mpa_info(mpa_frm *);
#endif				// DEBUG

static int rtp_uninit_parser(rtp_ssrc * stm_src, unsigned pt)
{
	rtp_mpa *mpa_priv = stm_src->privs[pt];

	if (mpa_priv) {
		nms_printf(NMSML_DBG2,
			   "[rtp_mpa] freeing private resources...\n");
		free(mpa_priv->data);
		free(mpa_priv);
	}

	return 0;
}

static int rtp_parse(rtp_ssrc * stm_src, rtp_frame * fr, rtp_buff * config)
{
	rtp_mpa *mpa_priv = stm_src->privs[fr->pt];
	rtp_pkt *pkt;
	size_t pkt_len;		// size of RTP packet, rtp header included.
	mpa_frm mpa;
	uint8 *mpa_data;

	if (!fr)
		return RTP_IN_PRM_ERR;

	// XXX should we check if payload/mime-type are compatible with parser?

	if (!(pkt = rtp_get_pkt(stm_src, &pkt_len)))
		return RTP_BUFF_EMPTY;

	// fr->timestamp = RTP_PKT_TS(pkt);

	// discard pkt if it's fragmented and the first fragment was lost
	while (RTP_MPA_PKT(pkt)->frag_offset) {
		rtp_rm_pkt(stm_src);
		if (!(pkt = rtp_get_pkt(stm_src, &pkt_len)))
			return RTP_BUFF_EMPTY;
		else if (RTP_PKT_PT(pkt) != fr->pt)
			return RTP_PARSE_ERROR;
	}

	mpa_data = RTP_MPA_PKT(pkt)->data;

	nms_printf(NMSML_DBG3, "--- fr->len: %d-%d\n", pkt_len,
		   RTP_PAYLOAD_SIZE(pkt, pkt_len));
	pkt_len = RTP_MPA_DATA_LEN(pkt, pkt_len);	// now pkt_len is only the payload len (excluded mpa subheader)
	nms_printf(NMSML_DBG3, "--- fr->len: %d\n", pkt_len);
	if (mpa_sync(&mpa_data, &pkt_len))
		return RTP_PARSE_ERROR;

	if (mpa_decode_header(mpa_data, &mpa))
		return RTP_PARSE_ERROR;

	/* XXX if the frame is not fragmented we could use directly the data contained in bufferpool 
	 * instead of memcpy the frame in a newly allocated space */
	// init private struct if this is the first time we're called
	if (!mpa_priv) {
		nms_printf(NMSML_DBG3,
			   "[rtp_mpa] allocating new private struct...");
		if (!
		    (stm_src->privs[fr->pt] = mpa_priv =
		     malloc(sizeof(rtp_mpa))))
			return RTP_ERRALLOC;
		mpa_priv->data_size = max(DEFAULT_MPA_DATA_FRAME, mpa.frm_len);
		if (!(mpa_priv->data = malloc(mpa_priv->data_size)))
			return RTP_ERRALLOC;
		rtp_parser_set_uninit(stm_src->rtp_sess, fr->pt,
				      rtp_uninit_parser);
		nms_printf(NMSML_DBG3, "done\n");
	} else if (mpa_priv->data_size < pkt_len) {
		nms_printf(NMSML_DBG3, "[rtp_mpa] reallocating data...");
		if ((mpa_priv->data = realloc(mpa_priv->data, mpa.frm_len)))
			return RTP_ERRALLOC;
		nms_printf(NMSML_DBG3, "done\n");
	}
	fr->data = mpa_priv->data;

	for (fr->len = 0;
	     pkt && (fr->len < mpa.frm_len) &&
	     (fr->timestamp == RTP_PKT_TS(pkt));
	     fr->len += pkt_len,
	     rtp_rm_pkt(stm_src), (pkt =
				   rtp_get_pkt(stm_src, &pkt_len)), pkt_len =
	     RTP_MPA_DATA_LEN(pkt, pkt_len)) {
		// pkt consistency checks
		if (RTP_MPA_FRAG_OFFSET(pkt) + pkt_len <= mpa_priv->data_size) {
			nms_printf(NMSML_DBG3,
				   "copying %d byte of data to offset: %d\n",
				   pkt_len, RTP_MPA_FRAG_OFFSET(pkt));
			memcpy(fr->data + RTP_MPA_FRAG_OFFSET(pkt), mpa_data,
			       pkt_len);
		}
	}
	nms_printf(NMSML_DBG3, "fr->len: %d\n", fr->len);

	return RTP_FILL_OK;
}

// private functions
static int mpa_sync(uint8 ** data, size_t * data_len /*, mpa_frm *mpa */ )
{
#if 0				// ID3 tag check not useful
	int ret;

	if (!mpa->probed) {
		/*look if ID3 tag is present */
		if (!memcmp(*data, "ID3", 3)) {	// ID3 tag present
			id3v2_hdr id3hdr;

			// fnc_log(FNC_LOG_DEBUG, "ID3v2 tag present in %s\n", i_stream->name);

			memcpy(&id3hdr, *data, 4);
			// if ( (ret = istream_read(ID3v2_HDRLEN - 4, &id3hdr.rev, i_stream)) != ID3v2_HDRLEN - 4 )
			if ((ret =
			     mpa_read(ID3v2_HDRLEN - 4, &id3hdr.rev,
				      in)) != ID3v2_HDRLEN - 4)
				return (ret < 0) ? ERR_PARSE : ERR_EOF;
			// if ( (ret=mpa_read_id3v2(&id3hdr, i_stream, mpa)) )
			if ((ret = mpa_read_id3v2(&id3hdr, in, mpa)))
				return ret;
			// if ( (ret=istream_read(4, *data, i_stream)) != 4 )
			if ((ret = mpa_read(4, *data, in)) != 4)
				return (ret < 0) ? ERR_PARSE : ERR_EOF;
		}
	}
#endif

	for (; !MPA_IS_SYNC(*data) && (*data_len >= 4);
	     (*data)++, (*data_len)--)
		nms_printf(NMSML_DBG3, "[MPA] sync: %X%X%X%X\n", data[0],
			   data[1], data[2], data[3]);

	return (*data_len >= 4) ? 0 /*sync found */ : 1 /*sync not found */ ;
}

static int mpa_decode_header(uint8 * buff_data, mpa_frm * mpa)
{
	int RowIndex, ColIndex;
//      uint8 *buff_data = (uint8 * )&header;
	int padding;
	int BitrateMatrix[16][5] = BITRATEMATRIX;
	float SRateMatrix[4][3] = SRATEMATRIX;

	if (!MPA_IS_SYNC(buff_data))
		return RTP_PARSE_ERROR;	/*syncword not found */

	mpa->id = (buff_data[1] & 0x18) >> 3;
	mpa->layer = (buff_data[1] & 0x06) >> 1;

	switch (mpa->id << 2 | mpa->layer) {
	case MPA_MPEG_1 << 2 | MPA_LAYER_I:	// MPEG 1 layer I
		ColIndex = 0;
		break;
	case MPA_MPEG_1 << 2 | MPA_LAYER_II:	// MPEG 1 layer II
		ColIndex = 1;
		break;
	case MPA_MPEG_1 << 2 | MPA_LAYER_III:	// MPEG 1 layer III
		ColIndex = 2;
		break;
	case MPA_MPEG_2 << 2 | MPA_LAYER_I:	// MPEG 2 layer I
	case MPA_MPEG_2_5 << 2 | MPA_LAYER_I:	// MPEG 2.5 layer I
		ColIndex = 3;
		break;
	case MPA_MPEG_2 << 2 | MPA_LAYER_II:	// MPEG 2 layer II
	case MPA_MPEG_2 << 2 | MPA_LAYER_III:	// MPEG 2 layer III
	case MPA_MPEG_2_5 << 2 | MPA_LAYER_II:	// MPEG 2.5 layer II
	case MPA_MPEG_2_5 << 2 | MPA_LAYER_III:	// MPEG 2.5 layer III
		ColIndex = 4;
		break;
	default:
		return RTP_PARSE_ERROR;
		break;
	}

	RowIndex = (buff_data[2] & 0xf0) >> 4;
	if (RowIndex == 0xF)	// bad bitrate
		return RTP_PARSE_ERROR;
	if (RowIndex == 0)	// free bitrate: not supported
		return RTP_PARSE_ERROR;
	mpa->bit_rate = BitrateMatrix[RowIndex][ColIndex];

	switch (mpa->id) {
	case MPA_MPEG_1:
		ColIndex = 0;
		break;
	case MPA_MPEG_2:
		ColIndex = 1;
		break;
	case MPA_MPEG_2_5:
		ColIndex = 2;
		break;
	default:
		return RTP_PARSE_ERROR;
		break;
	}

	RowIndex = (buff_data[2] & 0x0c) >> 2;
	if (RowIndex == 3)	// reserved
		return RTP_PARSE_ERROR;
	mpa->sample_rate = SRateMatrix[RowIndex][ColIndex];

	// padding
	padding = (buff_data[2] & 0x02) >> 1;

	// packet len
	if (mpa->layer == MPA_LAYER_I) {	// layer 1
		mpa->frame_size = 384;
		mpa->frm_len =
		    ((12 * mpa->bit_rate) / mpa->sample_rate + padding) * 4;
	} else {		// layer 2 or 3
		mpa->frame_size = 1152;
		mpa->frm_len =
		    144 * mpa->bit_rate / mpa->sample_rate + padding;
	}

#ifdef ENABLE_DEBUG
	mpa_info(mpa);
#endif				// DEBUG

	return 0;		// count; /*return 4*/
}

#ifdef ENABLE_DEBUG
// debug function to diplay MPA header information
static void mpa_info(mpa_frm * mpa)
{
	switch (mpa->id) {
	case MPA_MPEG_1:
		nms_printf(NMSML_DBG3, "[MPA] MPEG1\n");
		break;
	case MPA_MPEG_2:
		nms_printf(NMSML_DBG3, "[MPA] MPEG2\n");
		break;
	case MPA_MPEG_2_5:
		nms_printf(NMSML_DBG3, "[MPA] MPEG2.5\n");
		break;
	default:
		nms_printf(NMSML_DBG3, "[MPA] MPEG reserved (bad)\n");
		return;
		break;
	}
	switch (mpa->layer) {
	case MPA_LAYER_I:
		nms_printf(NMSML_DBG3, "[MPA] Layer I\n");
		break;
	case MPA_LAYER_II:
		nms_printf(NMSML_DBG3, "[MPA] Layer II\n");
		break;
	case MPA_LAYER_III:
		nms_printf(NMSML_DBG3, "[MPA] Layer III\n");
		break;
	default:
		nms_printf(NMSML_DBG3, "[MPA] Layer reserved (bad)\n");
		return;
		break;
	}
	nms_printf(NMSML_DBG3,
		   "[MPA] bitrate: %d; sample rate: %3.0f; pkt_len: %d\n",
		   mpa->bit_rate, mpa->sample_rate, mpa->frm_len);
}
#endif				// DEBUG
