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

struct RTSP_Session *rtsp_sess_copy(struct RTSP_Session *curr_rtsp_s)
{
	struct RTSP_Session *new_rtsp_s;

	if ( (new_rtsp_s=(struct RTSP_Session *)malloc(sizeof(struct RTSP_Session))) == NULL ) {
		nmsprintf(NMSML_FATAL, "Cannot allocate memory.\n");
		return NULL;
	}

	memcpy(new_rtsp_s, curr_rtsp_s, sizeof(struct RTSP_Session));

	new_rtsp_s->Session_ID=0;
	new_rtsp_s->next=NULL;

	return new_rtsp_s;
}
