/* * 
 *  $Id$
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

int handle_teardown_response(struct RTSP_Thread *rtsp_th)
{
	char *prev_tkn;

	if ((prev_tkn = strtok((rtsp_th->in_buffer).data, "\n")) == NULL) {
		nmsprintf(1, "Invalid RTSP-TEARDOWN response\n");
		rtsp_th->busy=0;
		return 1;
	}
	if ( check_status(prev_tkn, rtsp_th) ){
		remove_pkt(rtsp_th);
		rtsp_th->busy=0;
		return 1;
	}
	
	remove_pkt(rtsp_th);
	memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));

	return 0;
}

