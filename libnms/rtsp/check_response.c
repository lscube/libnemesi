/* * 
 *  $Id:check_response.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/methods.h>
#include <nemesi/utils.h>

int check_response(rtsp_thread * rtsp_th)
{
	int wait_res;
	uint64 wait_s_id;
	int wait_cseq;
	char *str_pos;
	int CSeq;
	uint64 Session_ID = 0;
	int opcode = 0;

	sscanf(rtsp_th->waiting_for, "%d", &wait_res);
	/* cerco il numero di sequenza del pacchetto arrivato */
	if ((str_pos = strstrcase((rtsp_th->in_buffer).data, "CSeq")) == NULL) {
		nms_printf(NMSML_ERR,
			   "ERROR: CANNOT find CSeq number in server response.\n");
		return 1;
	}
	str_pos += 5;
	while ((*(str_pos) == ' ') || (*(str_pos) == ':'))
		str_pos++;
	sscanf(str_pos, "%d", &CSeq);
	switch (wait_res) {
	case RTSP_GET_RESPONSE:
		if (CSeq == 1)	/* aspettavo la risposta alla DESCRIBE */
			opcode = RTSP_GET_RESPONSE;
		break;
	case RTSP_SETUP_RESPONSE:
		sscanf(rtsp_th->waiting_for, "%*d.%d", &wait_cseq);
		if (CSeq == wait_cseq)
			opcode = RTSP_SETUP_RESPONSE;
		break;
	case RTSP_CLOSE_RESPONSE:
		sscanf(rtsp_th->waiting_for, "%*d.%d", &wait_cseq);
		if (CSeq == wait_cseq)
			opcode = RTSP_CLOSE_RESPONSE;
		break;
	default:
		sscanf(rtsp_th->waiting_for, "%*d:%llu.%d", &wait_s_id,
		       &wait_cseq);
		if ((str_pos =
		     strstrcase((rtsp_th->in_buffer).data,
				"Session")) != NULL) {
			str_pos += 8;
			while ((*(str_pos) == ' ') || (*(str_pos) == ':'))
				str_pos++;
			sscanf(str_pos, "%llu", &Session_ID);
		}
		if ((Session_ID == wait_s_id) && (CSeq == wait_cseq))
			opcode = wait_res;
	}

	return opcode;
}
