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

int set_rtsp_sessions(struct RTSP_Thread *rtsp_th, int content_length, char *content_base, char *body)
{
	SDP_attr *sdp_a;
	char *tkn;
	
	switch (rtsp_th->descr_fmt) {
		case DESCRIPTION_SDP_FORMAT :
			if ( !(rtsp_th->rtsp_queue=rtsp_sess_create(rtsp_th->urlname, content_base)) )
				return 1;

			if ( !(rtsp_th->rtsp_queue->body=(char *)malloc((content_length+1)*sizeof(char))) )
				return nmserror("Cannot allocate memory.");
			memcpy(rtsp_th->rtsp_queue->body, body, content_length*sizeof(char));
			rtsp_th->rtsp_queue->body[content_length]='\0';

			rtsp_th->type=M_ON_DEMAND;

			if (!(rtsp_th->rtsp_queue->info=sdp_session_setup(rtsp_th->rtsp_queue->body, content_length)))
				return nmserror("SDP parse error");

			// we look for particular attributes of session
			for(sdp_a=rtsp_th->rtsp_queue->info->attr_list; sdp_a; sdp_a=sdp_a->next) {
				if ( !strncmpcase(sdp_a->a, "control", 7) ) {
					tkn = sdp_a->a + 7; // 7 == strlen("control")
					while ( (*tkn==' ') || (*tkn==':') ) // skip spaces and colon
						tkn++;
					rtsp_th->rtsp_queue->pathname=tkn;
					rtsp_th->type=CONTAINER;
				}
			}

			// media setup
			if (set_rtsp_media(rtsp_th))
				return 1;

			// for (tmp_sdp=curr_rtsp_s->)
#if 0
			do {
				if ( tkn==NULL )
					tkn=strtok(curr_rtsp_s->body, "\r\n");
				else
					tkn=strtok(NULL, "\r\n");
				if ( tkn==NULL ) {
					nmsprintf(NMSML_ERR, "Invalid RTSP-DESCRIBE body... discarding\n");
					return 1;
				}
				switch (*tkn)
				{
					case 'v':
						(curr_rtsp_s->info).v=tkn+2;
						break;
					case 'o':
						(curr_rtsp_s->info).o=tkn+2;
						break;
					case 's':
						(curr_rtsp_s->info).s=tkn+2;
						break;
					case 'i':
						(curr_rtsp_s->info).i=tkn+2;
						break;
					case 'u':
						(curr_rtsp_s->info).u=tkn+2;
						break;
					case 'e':
						(curr_rtsp_s->info).e=tkn+2;
						break;
					case 'p':
						(curr_rtsp_s->info).p=tkn+2;
						break;
					case 'c':
						(curr_rtsp_s->info).c=tkn+2;
						break;
					case 'b':
						(curr_rtsp_s->info).b=tkn+2;
						break;
					case 't':
						(curr_rtsp_s->info).t=tkn+2;
						break;
					case 'r':
						(curr_rtsp_s->info).r=tkn+2;
						break;
					case 'z':
						(curr_rtsp_s->info).z=tkn+2;
						break;
					case 'k':
						(curr_rtsp_s->info).k=tkn+2;
						break;
					case 'a':
						tkn+=2;
						if ( !strncmpcase(tkn, "control", 7) ) {
							tkn+=7;
							while ( (*(tkn)==' ') || (*(tkn)==':') )
								tkn++;
							curr_rtsp_s->pathname=tkn;
							rtsp_th->type=CONTAINER;
						}
						break;
					case 'm':
						tkn[strlen(tkn)]='\n';
						if ( set_rtsp_media(rtsp_th, content_length-(tkn-curr_rtsp_s->body), \
									&tkn) )
							return 1;
						break;
				}
			} while ( (tkn+strlen(tkn)-curr_rtsp_s->body+2)<content_length );
#endif // comment
			break;
		case DESCRIPTION_MH_FORMAT :
			/* not yet implemented */
			// break;
		default :
			nmsprintf(NMSML_ERR, "Unknown decription format.\n");
			return 1;
			break;
	}

	return 0;
}
