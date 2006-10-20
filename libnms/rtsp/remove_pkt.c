/* * 
 *  $Id:remove_pkt.c 267 2006-01-12 17:19:45Z shawill $
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

int remove_pkt(rtsp_thread * rtsp_th)
{

	char *buff = NULL;

	if ((rtsp_th->in_buffer.size - rtsp_th->in_buffer.first_pkt_size)) {
		if ((buff =
		     (char *) malloc(rtsp_th->in_buffer.size -
				     rtsp_th->in_buffer.first_pkt_size)) ==
		    NULL)
			return nms_printf(NMSML_FATAL,
					  "remove_pkt: Cannot allocate memory!\n");

		memcpy(buff,
		       rtsp_th->in_buffer.data +
		       rtsp_th->in_buffer.first_pkt_size,
		       rtsp_th->in_buffer.size -
		       rtsp_th->in_buffer.first_pkt_size);
	} else
		buff = NULL;
	free(rtsp_th->in_buffer.data);
	rtsp_th->in_buffer.data = buff;
	rtsp_th->in_buffer.size -= rtsp_th->in_buffer.first_pkt_size;
	rtsp_th->in_buffer.first_pkt_size = 0;
	return 0;
}
