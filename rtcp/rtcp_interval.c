/* * 
 *  ./rtcp/rtcp_interval.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:13 $
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

double rtcp_interval(int members, int senders, double rtcp_bw, int we_sent, double avg_rtcp_size, int initial)
{
	double t;
	double rtcp_min_time=RTCP_MIN_TIME;
	int n;

	if (initial)
		rtcp_min_time /= 2;

	n = members;
	if ( senders > 0 && senders < members*RTCP_SENDER_BW_FRACTION){
		if (we_sent){
			rtcp_bw *= RTCP_SENDER_BW_FRACTION;
			n=senders;
		} else {
			rtcp_bw *= RTCP_RCVR_BW_FRACTION;
			n -= senders;
		}
	}
	if ( (t = avg_rtcp_size * n / rtcp_bw) < rtcp_min_time )
		t = rtcp_min_time;
	t = (t * (drand48() + 0.5)) / COMPENSATION;
	return t;
}
