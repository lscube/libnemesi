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

#include <pthread.h>

#include <nemesi/video.h>
#include <nemesi/comm.h>

int video_th_stop(NMSVideo *vc)
{
	void *ret;

	if (vc && vc->tid) {
		nmsprintf(NMSML_DBG1, "Sending cancel signal to Video Thread (ID: %d)\n", vc->tid);
		if (pthread_cancel(vc->tid) != 0)
			nmsprintf(NMSML_DBG2, "Error while sending cancelation to Video Thread.\n");
		else
			pthread_join(vc->tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED )
			nmsprintf(NMSML_DBG2, "Warning! Video Thread joined, but  not canceled!\n");
		vc->tid = 0;
	}

	return 0;
}

