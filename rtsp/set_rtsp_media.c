/* * 
 *  ./rtsp/set_rtsp_media.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:49 $
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

int set_rtsp_media(struct RTSP_Thread *rtsp_th, int length, char **media_des)
{

	struct RTSP_Session *curr_rtsp_s=rtsp_th->rtsp_queue;
	struct RTSP_Medium *curr_rtsp_m=NULL;
	char *tkn=NULL, *ch;
	uint8 pt;
	

	do {
		if ( tkn==NULL )
			tkn=strtok(*media_des, "\r\n");
		else
			tkn=strtok(NULL, "\r\n");
		if ( tkn==NULL ) {
			uiprintf("Invalid Media description section.\n");
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
					if ( (curr_rtsp_s->next=rtsp_sess_copy(curr_rtsp_s))==NULL )
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
					/* Assumiamo che la stringa sia nella forma 
					 * rtpmap:PaloadType EncodingName/ClockRate/Channels */
					tkn+=6;
					while ( (*(tkn)==' ') || (*(tkn)==':') )
						tkn++;
					if ( ((pt=(uint8)strtoul(tkn, &tkn, 10)) >= 96) && ( pt <= 127 ) ){
						while ( *(tkn) == ' ' )
							tkn++;
						if ( (ch=strchr(tkn, '/')) == NULL ){
							uiprintf("Invalid field rtpmap.\n");
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
	return 0;
}
