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
#include <nemesi/cc.h>
#include <nemesi/ccprefs.h>
#include <nemesi/methods.h>
#include <nemesi/wsocket.h>

int send_play_request(struct RTSP_Thread *rtsp_th, char *range)
{
	char b[256];
	struct RTSP_Session *rtsp_sess;
	struct RTSP_Medium *rtsp_med;
	CCPermsMask cc_mask, cc_conflict;

	// get_curr_sess(NULL, &rtsp_sess, NULL);
	if (!(rtsp_sess=get_curr_sess(GCS_CUR_SESS)))
		return 1;
	
	// CC License check
	rtsp_med = rtsp_sess->media_queue;
	memset(&cc_conflict, 0, sizeof(cc_conflict));
	while ( rtsp_med ) {
		pref2ccmask(&cc_mask);
		if ( cc_prms_chk(rtsp_med->medium_info->cc, &cc_mask) )
			*((CC_BITMASK_T *)&cc_conflict) |= *((CC_BITMASK_T *)&cc_mask);
		rtsp_med = rtsp_med->next;
	}
	if (*((CC_BITMASK_T *)&cc_conflict)) {
		nmsprintf(NMSML_ERR, "You didn't accept some requested conditions of license:\n");
		cc_printmask(cc_conflict);
		return 1;
	}
	// end of CC part

	if ( rtsp_sess->content_base != NULL)
		if (*(rtsp_sess->pathname) != 0)
			sprintf(b, "%s %s/%s %s\nCSeq: %d\n", PLAY_TKN, rtsp_sess->content_base, rtsp_sess->pathname, RTSP_VER, ++(rtsp_sess->CSeq));
		else
			sprintf(b, "%s %s %s\nCSeq: %d\n", PLAY_TKN, rtsp_sess->content_base, RTSP_VER, ++(rtsp_sess->CSeq));
	else
		sprintf(b, "%s %s %s\nCSeq: %d\n", PLAY_TKN, rtsp_sess->pathname, RTSP_VER, ++(rtsp_sess->CSeq));
		
	if (rtsp_sess->Session_ID != 0)	/*must add session ID? */
		sprintf(b + strlen(b), "Session: %llu\n", rtsp_sess->Session_ID);
	if (range && *range)
		sprintf(b + strlen(b), "Range: %s\n", range);
	else
		sprintf(b + strlen(b), "Range: time=0-\n");
	
	strcat(b, "\r\n");

	if (!tcp_write(rtsp_th->fd, b, strlen(b))) {
		nmsprintf(NMSML_ERR, "Cannot send PLAY request...\n");
		return 1;
	}

	sprintf(rtsp_th->waiting_for, "%d:%llu.%d", RTSP_PLAY_RESPONSE, rtsp_sess->Session_ID, rtsp_sess->CSeq);

	return 0;
}
