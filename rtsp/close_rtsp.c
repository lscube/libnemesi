/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
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

#include <nemesi/rtsp.h>

int close_rtsp(struct RTSP_Ctrl *rtsp_ctrl)
{
	void *ret=NULL;

	/* THREAD CANCEL */	
	nmsprintf(2, "Sending cancel signal to all threads\n");
	if(rtsp_ctrl->rtsp_tid > 0){
		nmsprintf(2, "Sending cancel signal to RTSP Thread (ID: %lu)\n", rtsp_ctrl->rtsp_tid);
		if (pthread_cancel(rtsp_ctrl->rtsp_tid) != 0)
			nmsprintf(3, "Error while sending cancelation to RTSP Thread.\n");
		else
			pthread_join(rtsp_ctrl->rtsp_tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED ) {
			nmsprintf(3, "Warning! RTSP Thread joined, but  not canceled!\n");
			return 1;
		}
	} else {
		nmsprintf(2, "Cannot send cancel signal to RTSP Thread\n");
		return 1;
	}

	return 0;
}

