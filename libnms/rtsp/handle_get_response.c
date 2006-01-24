/* * 
 *  $Id:handle_get_response.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/utils.h>
#include <nemesi/methods.h>

int handle_get_response(struct rtsp_thread *rtsp_th)
{
	char *tkn; /* contains an RTSP description line */
	char *prev_tkn; /* addresses first the previous token in order to check the end of RTSP header 
			   and then the all the components of command line */
	int content_length;
	char *content_base=NULL;

	if ( (prev_tkn=strtok((rtsp_th->in_buffer).data,"\n"))==NULL ) {
		nms_printf(NMSML_ERR, "Invalid RTSP-DESCRIBE response\n");
		return 1;
	}
	if ( check_status(prev_tkn, rtsp_th) < 0 ){
		remove_pkt(rtsp_th);
		return 1;
	}
	/* state success: header parsing */
	while ( ((tkn=strtok(NULL, "\n")) != NULL) && ((tkn-prev_tkn)>1) ) {
		if ( ((tkn-prev_tkn)==2) && (*prev_tkn=='\r') )
			break;
		prev_tkn=tkn;
		if ( !strncmpcase(prev_tkn,"Content-Length", 14) ) {
			prev_tkn+=14;
			while ( (*(prev_tkn)==' ') || (*(prev_tkn)==':') )
				prev_tkn++;
			sscanf(prev_tkn,"%d",&content_length);
		}
		else if ( !strncmpcase(prev_tkn, "Content-Type",12) ) {
			prev_tkn+=12;
			while ( (*(prev_tkn)==' ') || (*(prev_tkn)==':') )
				prev_tkn++;
			if ( !strncmpcase(prev_tkn, "application/sdp", 15) )
				rtsp_th->descr_fmt=DESCRIPTION_SDP_FORMAT;
				// description_format=DESCRIPTION_SDP_FORMAT;
			/* 
			else if ( !strncmpcase(prev_tkn, "application/x-rtsp-mh", 21) )
				description_format=DESCRIPTION_MH_FORMAT;
			 */
			else {
				nms_printf(NMSML_ERR, "Content-Type %s not recognized\n", prev_tkn);
			}
		}
		else if ( !strncmpcase(prev_tkn, "Content-Base", 12) ) {
			prev_tkn+=12;
			while ( (*(prev_tkn)==' ') || (*(prev_tkn)==':') )
				prev_tkn++;
			content_base=prev_tkn;
			if ( content_base[strlen(content_base)-1]=='\r' )
				content_base[strlen(content_base)-1]='\0';
			if ( content_base[strlen(content_base)-1]=='/' )
				content_base[strlen(content_base)-1]='\0';
		}
	}
	while ( (tkn!=NULL) && ((*tkn=='\r') || (*tkn=='\n') || (*tkn=='\0')) )
		tkn=strtok(NULL, "\n"); /* cerco l'inizio del body o, eventualmente del prossimo pkt */
	if ( tkn!=NULL )
		tkn[strlen(tkn)]='\n'; /* rimetto a posto il \n modificato dalla strtok */
	// if ( set_rtsp_sessions(rtsp_th, content_length, content_base, tkn, description_format) ) {
	if ( set_rtsp_sessions(rtsp_th, content_length, content_base, tkn) )
		return 1;
	remove_pkt(rtsp_th);
	memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));
	return 0;
}
