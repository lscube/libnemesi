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
 
 #include <nemesi/decoder.h>

int dec_destroy(pthread_t tid)
{
	void *ret;

	if (tid) {
		nms_printf(NMSML_DBG1, "Sending cancel signal to Decoder (ID: %u)\n", tid);
		if (pthread_cancel(tid) != 0)
			nms_printf(NMSML_DBG2, "Error while sending cancelation to Decoder Thread.\n");
		else
			pthread_join(tid, (void **)&ret);
		if ( ret != PTHREAD_CANCELED )
			nms_printf(NMSML_DBG2, "Warning! Decoder Thread joined, but not canceled!\n");
	}

	return 0;
}
