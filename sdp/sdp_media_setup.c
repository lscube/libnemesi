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
					if (!(curr_sdp_m->next=(SDP_Medium_info *)calloc(1, sizeof(SDP_Medium_info))))
						return NULL;
					curr_sdp_m=curr_sdp_m->next;
				}
				curr_sdp_m->m=tkn+2;

#if 0
				if ( curr_sdp_m==NULL ) {
					/* primo medium */
					if ( ( curr_rtsp_s->media_queue=curr_rtsp_m=rtsp_med_create(rtsp_th->fd) )==NULL )
						return 1;
				} else if ( rtsp_th->type==CONTAINER ) {
					/* media nella stessa sessione */
					if ( (curr_rtsp_m->next=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_m->next->rtp_sess;
					curr_rtsp_m=curr_rtsp_m->next;
				} else if ( rtsp_th->type==M_ON_DEMAND ) {
					/* una sessione per ogni medium */
					if ( (curr_rtsp_s->next=rtsp_sess_copy(curr_rtsp_s))==NULL )
						return 1;
					curr_rtsp_s=curr_rtsp_s->next;
					if ( (curr_rtsp_s->media_queue=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_s->media_queue->rtp_sess;
					curr_rtsp_m=curr_rtsp_s->media_queue;
				}
				curr_sdp_m->m=tkn+2;
#endif
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
					return NULL;
				}
#if 0
				if ( !strncmpcase(tkn, "control", 7) ) {
					tkn+=7;
					while ( (*(tkn)==' ') || (*(tkn)==':') )
						tkn++;
					curr_rtsp_m->filename=tkn;
				}
				if ( !strncmpcase(tkn, "rtpmap", 6) ) {
					/* We assume the string in the format:
					 * rtpmap:PaloadType EncodingName/ClockRate/Channels */
					tkn+=6;
					while ( (*(tkn)==' ') || (*(tkn)==':') )
						tkn++;
					if ( ((pt=(uint8)strtoul(tkn, &tkn, 10)) >= 96) && ( pt <= 127 ) ){
						while ( *(tkn) == ' ' )
							tkn++;
						if ( (ch=strchr(tkn, '/')) == NULL ){
							nmsprintf(2, "Invalid field rtpmap.\n");
							break;
						}
						if ( (ch - tkn) > (RTP_DEF_MAX_NAME_LEN - 1) ){
							strncpy(rtp_pt_defs[pt].name, tkn, (RTP_DEF_MAX_NAME_LEN - 1));
							rtp_pt_defs[pt].name[(RTP_DEF_MAX_NAME_LEN - 1)]=0;
						}
						else {
							strncpy(rtp_pt_defs[pt].name, tkn, ch - tkn);
							rtp_pt_defs[pt].name[ch-tkn]=0;
						}
						tkn=++ch;
						sscanf(tkn, "%u/%u", (unsigned *)&(rtp_pt_defs[pt].rate), (unsigned *)&(rtp_pt_defs[pt].channels));
						rtp_pt_defs[pt].type=NA;
					}
				}
#endif
				break;
		}
	} while ( (tkn+strlen(tkn)-*descr+2)<descr_len );
	*descr=tkn;

	return queue;
}

