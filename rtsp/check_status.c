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
#include <nemesi/utils.h>

/*!
 * \brief scan status code of an RTSP reply
 *
 * \param status_line the status line in the reply
 * \return reply status code or -1 on error
 */
int check_status(char *status_line, struct RTSP_Thread *rtsp_th)
{
	char ver[32];
	unsigned short res_state;
	char *reason_phrase;
	char *location=NULL;
	// string tokenizers
	char *tkn, *prev_tkn;

	if ( sscanf(status_line, "%s %hu ", ver, &res_state) < 2) {
		nmsprintf(1, "invalid Status-Line in DESCRIBE Response\n");
		return -1;
	}
	reason_phrase=strchr(strchr(status_line, ' ')+1, ' ')+1;

	if (RTSP_IS_SUCCESS(res_state))
		return res_state;
	// if ( (res_state>=300) && (res_state<400) ) {
	if ( RTSP_IS_REDIRECT(res_state) ) {
		nmsprintf(1, "WARNING: Redirection. reply was: %hu %s\n", res_state, reason_phrase);
		// prev_tkn = status_line;
		if ( (prev_tkn=strtok((rtsp_th->in_buffer).data + strlen(status_line) + 1,"\n"))==NULL ) {
			nmsprintf(1, "Could not find \"Location\" so... were I'll redirect you?\n");
			return -1;
		}
		while ( ((tkn=strtok(NULL, "\n")) != NULL) && ((tkn-prev_tkn)>1) ) {
			if ( ((tkn-prev_tkn)==2) && (*prev_tkn=='\r') )
				break;
			if ( !strncmpcase(prev_tkn, "Location", 8) ) {
				prev_tkn+=8;
				while ( (*(prev_tkn)==' ') || (*(prev_tkn)==':') )
					prev_tkn++;
				location = strdup(prev_tkn);
				// sscanf(prev_tkn,"%d",&location);
			}
			prev_tkn=tkn;
		}
		if (location) {
			nmsprintf(1, "Redirecting to %s\n", location);
			// XXX:proving
			pthread_mutex_lock(&(rtsp_th->comm_mutex));
			rtsp_th->comm->opcode = OPEN;
			write(rtsp_th->pipefd[1], "o", 1);
			strncpy(rtsp_th->comm->arg, location, sizeof(rtsp_th->comm->arg));
			rtsp_th->busy=1;
			pthread_mutex_unlock(&(rtsp_th->comm_mutex));
			///// XXX: end proving
		} else
			return -nmserror("No location string");
		// rtsp_th->status=INIT;
	}
	// if ( (res_state>=400) && (res_state<500))
	if ( RTSP_IS_CLIENT_ERROR(res_state) )
		nmsprintf(1, "WARNING: Client error. Reply was: %hu %s\n", res_state, reason_phrase);
	// if ( res_state>=500 )
	if ( RTSP_IS_SERVER_ERROR(res_state) )
		nmsprintf(1, "WARNING; Server error. Reply was: %hu %s\n", res_state, reason_phrase);
	return -1;
}
