/* * 
 *  $Id:set_rtsp_media.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/rtpptdefs.h>

int set_rtsp_media(struct rtsp_thread *rtsp_th)
{

	struct rtsp_session *curr_rtsp_s=rtsp_th->rtsp_queue;
	struct rtsp_medium *curr_rtsp_m=NULL;
	sdp_medium_info *sdp_m;
	sdp_attr *sdp_attr;
	char *tkn, *ch;
	uint8 pt;
	

	switch (rtsp_th->descr_fmt) {
		case DESCRIPTION_SDP_FORMAT :
			for(sdp_m=curr_rtsp_s->info->media_info_queue; sdp_m; sdp_m=sdp_m->next) {
				if ( curr_rtsp_m==NULL ) {
					/* first medium */
					if ( ( curr_rtsp_s->media_queue=curr_rtsp_m=rtsp_med_create(rtsp_th->fd) )==NULL )
						return 1;
				} else if ( rtsp_th->type==CONTAINER ) {
					/* media in the same session */
					if ( (curr_rtsp_m->next=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_m->next->rtp_sess;
					curr_rtsp_m=curr_rtsp_m->next;
				} else if ( rtsp_th->type==M_ON_DEMAND ) {
					/* one medium for each session */
					if ( (curr_rtsp_s->next=rtsp_sess_dup(curr_rtsp_s))==NULL )
						return 1;
					curr_rtsp_s=curr_rtsp_s->next;
					if ( (curr_rtsp_s->media_queue=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_s->media_queue->rtp_sess;
					curr_rtsp_m=curr_rtsp_s->media_queue;
				}
				curr_rtsp_m->medium_info = sdp_m;
				for(sdp_attr=sdp_m->attr_list; sdp_attr; sdp_attr=sdp_attr->next) {
					if ( !strncmpcase(sdp_attr->a, "control", 7) ) {
						tkn = sdp_attr->a + 7; // 7 == strlen("control")
						while ( (*tkn==' ') || (*tkn==':') ) // skip spaces and colon
							tkn++;
						curr_rtsp_m->filename=tkn;
					}
					if ( !strncmpcase(sdp_attr->a, "rtpmap", 6) ) {
						/* We assume the string in the format:
					 	* rtpmap:PaloadType EncodingName/ClockRate[/Channels] */
						tkn = sdp_attr->a + 6; // 6 == strlen("rtpmap")
						// skip spaces and colon (we should not do this!)
						while ( (*tkn==' ') || (*tkn==':') )
							tkn++;
						if ( ((pt=(uint8)strtoul(tkn, &tkn, 10)) >= 96) && ( pt <= 127 ) ){
							while ( *tkn == ' ' )
								tkn++;
							if ( !(ch=strchr(tkn, '/')) ){
								nms_printf(NMSML_WARN, "Invalid field rtpmap.\n");
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

							if ( !strncmpcase(sdp_m->m, "audio", 5/*strlen("audio")*/) )
								rtp_pt_defs[pt].type=AU;
							else if ( !strncmpcase(sdp_m->m, "video", 5/*strlen("video")*/) )
								rtp_pt_defs[pt].type=VI;
							else
								rtp_pt_defs[pt].type=NA;
						} else {
							// shawill: should be an error or a warning?
							nms_printf(NMSML_WARN, "Warning: rtpmap attribute is trying to set a non-dynamic payload type: not permitted\n");
						}
					}
				}
			}
			break;
		case DESCRIPTION_MH_FORMAT :
			/* not yet implemented */
			// break;
		default :
			nms_printf(NMSML_ERR, "Unknown decription format.\n");
			return 1;
			break;
	}
#if 0
	do {
		if ( tkn==NULL )
			tkn=strtok(*media_des, "\r\n");
		else
			tkn=strtok(NULL, "\r\n");
		if ( tkn==NULL ) {
			nms_printf(NMSML_ERR, "Invalid Media description section.\n");
			return 1;
		}
		switch (*tkn) {
			case 'm':  /* creo la struttura per il nuovo medium */
				if ( curr_rtsp_m==NULL ) {
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
					if ( (curr_rtsp_s->next=rtsp_sess_dup(curr_rtsp_s))==NULL )
						return 1;
					curr_rtsp_s=curr_rtsp_s->next;
					if ( (curr_rtsp_s->media_queue=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_s->media_queue->rtp_sess;
					curr_rtsp_m=curr_rtsp_s->media_queue;
				}
				(curr_rtsp_m->medium_info).m=tkn+2;
				break;
			case 'i':
				(curr_rtsp_m->medium_info).i=tkn+2;
				break;
			case 'c':
				(curr_rtsp_m->medium_info).c=tkn+2;
				break;
			case 'b':
				(curr_rtsp_m->medium_info).b=tkn+2;
				break;
			case 'k':
				(curr_rtsp_m->medium_info).k=tkn+2;
				break;
			case 'a':
				tkn+=2;
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
							nms_printf(NMSML_WARN, "Invalid field rtpmap.\n");
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
				break;
		}
	} while ( (tkn+strlen(tkn)-*media_des+2)<length );
	*media_des=tkn;
#endif // comment
	return 0;
}
