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

#include <stdio.h>
#include <string.h>

#include <nemesi/sdp.h>
#include <nemesi/comm.h>

SDP_Medium_info *sdp_media_setup(char **descr, int descr_len)
{
	SDP_Medium_info *queue=NULL, *curr_sdp_m=NULL;
	char *tkn=NULL;
	char error=0; // error flag
	// for cc tagging
	int pt;
	char *endtkn = NULL;

	do {
		if ( tkn==NULL )
			tkn=strtok(*descr, "\r\n");
		else
			tkn=strtok(NULL, "\r\n");
		if ( tkn==NULL ) {
			nmsprintf(1, "Invalid SDP Media description section.\n");
			return NULL;
		}
		switch (*tkn) {
			case 'm':  /* create struct for new medium */
				if (!curr_sdp_m) { // first medium description
					// we use calloc, so it's all already initialized to NULL
					if (!(queue=curr_sdp_m=(SDP_Medium_info *)calloc(1, sizeof(SDP_Medium_info))))
						return NULL;
				} else { // not first medium in sdp session
					// we use calloc, so it's all already initialized to NULL
					if (!(curr_sdp_m->next=(SDP_Medium_info *)calloc(1, sizeof(SDP_Medium_info)))) {
						error=1;
						break;
						// return NULL;
					}
					curr_sdp_m=curr_sdp_m->next;
				}
				curr_sdp_m->m=tkn+2;
				if (sdp_parse_m_descr(curr_sdp_m, curr_sdp_m->m))
					error=1;
				break;
			case 'i':
				curr_sdp_m->i=tkn+2;
				break;
			case 'c':
				curr_sdp_m->c=tkn+2;
				break;
			case 'b':
				curr_sdp_m->b=tkn+2;
				break;
			case 'k':
				curr_sdp_m->k=tkn+2;
				break;
			case 'a':
				tkn+=2;
				if (sdp_set_attr(&(curr_sdp_m->attr_list), tkn)) {
					nmserror("Error setting SDP media atrtibute");
					error=1;
					break;
					// return NULL;
				}
				if (issdplicense(tkn)) {
					if (!curr_sdp_m->cc)
						if (!(curr_sdp_m->cc=cc_newlicense())) {
							nmserror("Could not get new CC license struct");
							error=1;
							break;
							// return NULL;
						}
					if (cc_set_sdplicense(curr_sdp_m->cc, tkn)) {
						error=1;
						break;
						// return NULL;
					}
				}
				break;
		}
	} while ( (!error) && ((tkn+strlen(tkn)-*descr+2) < descr_len) );
	*descr=tkn;

	if (error) { // there was an error?
		sdp_media_destroy(queue);
		return NULL;
	} else { // setup CC tags for disk writing
		for (curr_sdp_m=queue; curr_sdp_m; curr_sdp_m=curr_sdp_m->next) {
			for (tkn=curr_sdp_m->fmts; *tkn; tkn=endtkn) {
				for (;*tkn==' ';tkn++); // skip spaces
				pt = strtol(tkn, &endtkn, 10);
				if (tkn != endtkn)
					cc_setag(pt, curr_sdp_m->cc);
				else
					break;
			}
		}
	}

	return queue;
}

