/* * 
 *  $Id:rtcp_thread_create.c 267 2006-01-12 17:19:45Z shawill $
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

int rtcp_thread_create(rtp_thread * rtp_th)
{
	int n;
	pthread_attr_t rtcp_attr;

	pthread_attr_init(&rtcp_attr);
	if (pthread_attr_setdetachstate(&rtcp_attr, PTHREAD_CREATE_JOINABLE) != 0)
		return nms_printf(NMSML_FATAL, "Cannot set RTCP Thread attributes!\n");

	if ((n = pthread_create(&rtp_th->rtcp_tid, &rtcp_attr, &rtcp, (void *) rtp_th)) > 0)
		return nms_printf(NMSML_FATAL, "%s\n", strerror(n));

	return 0;
}
