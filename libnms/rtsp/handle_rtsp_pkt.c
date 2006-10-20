/* * 
 *  $Id:handle_rtsp_pkt.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtsp.h>

int handle_rtsp_pkt(rtsp_thread * rtsp_th)
{
	char ver[32];
	int opcode;
	

	if ((rtsp_th->transport.type == TCP && rtsp_th->interleaved) && (rtsp_th->in_buffer).data[0] == '$') {
		nms_rtsp_interleaved *p;
		const uint8 m = ((rtsp_th->in_buffer).data[1]);
#define DATA_PTR (&((rtsp_th->in_buffer).data[4]))
#define DATA_SIZE ((rtsp_th->in_buffer).first_pkt_size - 4)

		for (p = rtsp_th->interleaved; p && !((p->proto.tcp.rtp_ch == m)
			|| (p->proto.tcp.rtcp_ch == m)); p = p->next);
		if (p) {
			if (p->proto.tcp.rtcp_ch == m) {
				nms_printf(NMSML_DBG2,
					   "Interleaved RTCP data (%u bytes: channel %d -> sd %d)\n",
					   DATA_SIZE, m, p->rtcp_fd);
				send(p->rtcp_fd, DATA_PTR, DATA_SIZE, 0);
			} else {
				nms_printf(NMSML_DBG2,
					   "Interleaved RTP data (%u bytes: channel %d -> sd %d)\n",
					   DATA_SIZE, m, p->rtp_fd);
				send(p->rtp_fd, DATA_PTR, DATA_SIZE, 0);
			}
		}

		remove_pkt(rtsp_th);
		return 0;
	}

	if (sscanf((rtsp_th->in_buffer).data, "%s ", ver) < 1) {
		printf("\nInvalid RTSP message received\n");
		return 1;
	}
	if (strncmp(ver, "RTSP", 4) && strncmp(ver, "rtsp", 4)) {
		printf("\nInvalid RTSP message received\n");
		return 1;
	}
	if ((opcode = check_response(rtsp_th)) == 0) {
		printf("\nunexpected RTSP packet arrived\n");
		return 1;
	}
	/*
	   if (state_machine[rtsp_th->status] (rtsp_th, opcode))
	   return 1;
	 */
	return state_machine[rtsp_th->status] (rtsp_th, opcode);

	// return 0;
}
