/* * 
 *  $Id:rtcp_hdr_val_chk.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
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

#include <nemesi/rtcp.h>

int rtcp_hdr_val_chk(rtcp_pkt *pkt, int len)
{
	rtcp_pkt *end;

	if (len < (int)sizeof(rtcp_common_t)) {
		nms_printf(NMSML_ERR, "RTCP packet too small!!! (%d)\n", len);
		return 1;
	}

	if (len/4 > (ntohs((pkt->common).len) + 1)){
		/* This is a fu**ing compound pkt */
		nms_printf(NMSML_DBG2, "RTCP Compound packet arrived (total len=%d)\n", len);
		if ( ( *(uint16 *)pkt & RTCP_VALID_MASK) != RTCP_VALID_VALUE) {
			nms_printf(NMSML_WARN, "RTCP Header not valid: first pkt of Compound is not a SR (or RR)!\n"BLANK_LINE);
			return 1;
		}
		end = (rtcp_pkt *)((uint32 *)pkt + len/4);
		do pkt=(rtcp_pkt *)((uint32 *)pkt + ntohs((pkt->common).len) + 1);
		while ((pkt < end) && ((pkt->common).ver == 2));

		if (pkt != end) {
			nms_printf(NMSML_WARN, "RTCP Header not valid: mismatching lenght (%d)!\n"BLANK_LINE, len);
			return 1;
		}
	} else {
		nms_printf(NMSML_DBG2, "RTCP packet arrived (total len=%d)\n", len);
		if ( (pkt->common).ver != RTP_VERSION ){
			nms_printf(NMSML_WARN, "RTCP Header not valid: mismatching RTP version number!\n"BLANK_LINE);
			return 1;
		}
		if ( ! (((pkt->common).pt>=200) && ((pkt->common).pt<=204)) ){
			nms_printf(NMSML_WARN, "RTCP Header not valid: mismatching payload type!\n"BLANK_LINE);
			return 1;
		}
		if ( ((pkt->common).pad) && ( *(((uint8 *)pkt)+len-1) > (pkt->common).len*4) ){
			nms_printf(NMSML_WARN, "RTCP Header not valid: mismatching lenght!\n"BLANK_LINE);
			return 1;
		}
	}

	return 0;
}
