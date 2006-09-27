/* * 
 *  $Id:sdp_session_setup.c 267 2006-01-12 17:19:45Z shawill $
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

#include <stdio.h>
#include <string.h>

#include <nemesi/sdp.h>
#include <nemesi/comm.h>

sdp_session_info *sdp_session_setup(char *descr, int descr_len)
{
	sdp_session_info *new;
	char *tkn = NULL;
	char error = 0;		// error flag

	// we use calloc, so it's all already initialized to NULL
	if (!(new = (sdp_session_info *) calloc(1, sizeof(sdp_session_info))))
		return NULL;

	do {
		if (tkn == NULL)
			tkn = strtok(descr, "\r\n");
		else
			tkn = strtok(NULL, "\r\n");
		if (tkn == NULL) {
			nms_printf(NMSML_ERR,
				   "Invalid SDP description body... discarding\n");
			error = 1;
			break;
			// return NULL;
		}
		switch (*tkn) {
		case 'v':
			new->v = tkn + 2;
			break;
		case 'o':
			new->o = tkn + 2;
			break;
		case 's':
			new->s = tkn + 2;
			break;
		case 'i':
			new->i = tkn + 2;
			break;
		case 'u':
			new->u = tkn + 2;
			break;
		case 'e':
			new->e = tkn + 2;
			break;
		case 'p':
			new->p = tkn + 2;
			break;
		case 'c':
			new->c = tkn + 2;
			break;
		case 'b':
			new->b = tkn + 2;
			break;
		case 't':
			new->t = tkn + 2;
			break;
		case 'r':
			new->r = tkn + 2;
			break;
		case 'z':
			new->z = tkn + 2;
			break;
		case 'k':
			new->k = tkn + 2;
			break;
		case 'a':
			tkn += 2;
			if (sdp_set_attr(&(new->attr_list), tkn)) {
				nms_printf(NMSML_ERR,
					   "Error setting SDP session attribute\n");
				error = 1;
				break;
				// return NULL;
			}
			break;
		case 'm':
			tkn[strlen(tkn)] = '\n';
			if (!
			    (new->media_info_queue =
			     sdp_media_setup(&tkn,
					     descr_len - (tkn - descr)))) {
				error = 1;
				break;
				// return NULL;
			}
			break;
		}
	} while ((tkn + strlen(tkn) - descr + 2) < descr_len);

	if (error) {		// there was an error?
		sdp_session_destroy(new);
		return NULL;
	}

	return new;
}
