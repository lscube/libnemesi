/* * 
 *  $Id:close_cmd.c 267 2006-01-12 17:19:45Z shawill $
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
#include <stdarg.h>

int close_cmd(struct rtsp_thread *rtsp_th, ...)
{

	if(rtsp_th->status == INIT){
		nms_printf(NMSML_NORM, BLANK_LINE);
		nms_printf(NMSML_NORM, "No Connection to close\n");
		return 1;
	}
	// get_curr_sess(NULL, NULL, NULL);
	// get_curr_sess(GCS_UNINIT); // useless
	// get_curr_sess(rtsp_th, NULL, NULL);
	get_curr_sess(GCS_INIT, rtsp_th);
	if (send_teardown_request(rtsp_th))
		return 1;

	return 0;
}
