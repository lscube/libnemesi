/* * 
 *  ./rtsp/set_rtsp_sessions.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

int set_rtsp_sessions(struct RTSP_Thread *rtsp_th, int content_length, char *content_base, char *body, char description_format)
{
	enum boolean { false, true } media_des;
	char *tkn=NULL;
	
	switch (description_format)
	{
		case DESCRIPTION_SDP_FORMAT :
			if ( (rtsp_th->rtsp_queue=rtsp_sess_create(rtsp_th->urlname, content_base))==NULL )
				return 1;
			if ( (rtsp_th->rtsp_queue->body=(char *)malloc((content_length+1)*sizeof(char))) == NULL ) {
				uiprintf("Cannot allocate memory.\n");
				return 1;
			}
			memcpy(rtsp_th->rtsp_queue->body, body, content_length*sizeof(char));
			rtsp_th->rtsp_queue->body[content_length]='\0';
			media_des=false;
			rtsp_th->type=M_ON_DEMAND;
			do {
				if ( tkn==NULL )
					tkn=strtok(rtsp_th->rtsp_queue->body, "\r\n");
				else
					tkn=strtok(NULL, "\r\n");
				if ( tkn==NULL ) {
					uiprintf("Invalid RTSP-DESCRIBE body... discarding\n");
					return 1;
				}
				switch (*tkn)
				{
					case 'v':
						(rtsp_th->rtsp_queue->info).v=tkn+2;
						break;
					case 'o':
						(rtsp_th->rtsp_queue->info).o=tkn+2;
						break;
					case 's':
						(rtsp_th->rtsp_queue->info).s=tkn+2;
						break;
					case 'i':
						(rtsp_th->rtsp_queue->info).i=tkn+2;
						break;
					case 'u':
						(rtsp_th->rtsp_queue->info).u=tkn+2;
						break;
					case 'e':
						(rtsp_th->rtsp_queue->info).e=tkn+2;
						break;
					case 'p':
						(rtsp_th->rtsp_queue->info).p=tkn+2;
						break;
					case 'c':
						(rtsp_th->rtsp_queue->info).c=tkn+2;
						break;
					case 'b':
						(rtsp_th->rtsp_queue->info).b=tkn+2;
						break;
					case 't':
						(rtsp_th->rtsp_queue->info).t=tkn+2;
						break;
					case 'r':
						(rtsp_th->rtsp_queue->info).r=tkn+2;
						break;
					case 'z':
						(rtsp_th->rtsp_queue->info).z=tkn+2;
						break;
					case 'k':
						(rtsp_th->rtsp_queue->info).k=tkn+2;
						break;
					case 'a':
						tkn+=2;
						if ( !strncmpcase(tkn, "control", 7) ) {
							tkn+=7;
							while ( (*(tkn)==' ') || (*(tkn)==':') )
								tkn++;
							rtsp_th->rtsp_queue->pathname=tkn;
							rtsp_th->type=CONTAINER;
						}
						break;
					case 'm':
						tkn[strlen(tkn)]='\n';
						if ( set_rtsp_media(rtsp_th, content_length-(tkn-rtsp_th->rtsp_queue->body), \
									&tkn) )
							return 1;
						break;
				}
			} while ( (tkn+strlen(tkn)-rtsp_th->rtsp_queue->body+2)<content_length );
			break;
		case DESCRIPTION_MH_FORMAT :
			/* not yet implemented */
			break;
		default :
			uiprintf("Unknown decription format.\n");
			return 1;
			break;
	}
	return 0;
}
