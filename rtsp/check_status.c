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
#include <nemesi/methods.h>

int check_status(char *status_line, struct RTSP_Thread *rtsp_th)
{
	char ver[32];
	unsigned short res_state;
	char *reason_phrase;

	if ( sscanf(status_line, "%s %hu ", ver, &res_state) < 2) {
		nmsprintf(1, "invalid Status-Line in DESCRIBE Response\n");
		return 1;
	}
	reason_phrase=strchr(strchr(status_line, ' ')+1, ' ')+1;
	if ( (res_state>=300) && (res_state<400) ) {
		nmsprintf(1, "WARNING: Redirection. reply was: %hu %s -> INIT-STATE\n", res_state, reason_phrase);
		rtsp_th->status=INIT;
	}
	if ( (res_state>=400) && (res_state<500))
		nmsprintf(1, "WARNING: Client error. Reply was: %hu %s\n", res_state, reason_phrase);
	if ( res_state>=500 )
		nmsprintf(1, "WARNING; Server error. Reply was: %hu %s\n", res_state, reason_phrase);
	if ( (res_state>=200) && (res_state<300) )
		return 0;
	return 1;
}
