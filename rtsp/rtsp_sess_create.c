/* * 
 *  ./rtsp/rtsp_sess_create.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:49 $
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

struct RTSP_Session *rtsp_sess_create(char *urlname, char *content_base)
{
	struct RTSP_Session *rtsp_s;

	if ( (rtsp_s=(struct RTSP_Session *)malloc(sizeof(struct RTSP_Session))) == NULL ){
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}
	if ( content_base == NULL ){
			rtsp_s->content_base=NULL;
			rtsp_s->pathname=urlname;
	} else {
		if ((rtsp_s->pathname=rtsp_s->content_base=(char *)malloc(sizeof(char)*(strlen(content_base)+1)))==NULL){
			uiprintf("Cannot allocate memory!\n");
			return NULL;
		}
		strcpy(rtsp_s->content_base,content_base);
		rtsp_s->pathname+=strlen(content_base);
	} 
	rtsp_s->Session_ID=0;
	rtsp_s->CSeq=1;
	rtsp_s->media_queue=NULL;
	rtsp_s->next=NULL;

	(rtsp_s->info).v=NULL;
	(rtsp_s->info).o=NULL;
	(rtsp_s->info).s=NULL;
	(rtsp_s->info).i=NULL;
	(rtsp_s->info).u=NULL;
	(rtsp_s->info).e=NULL;
	(rtsp_s->info).p=NULL;
	(rtsp_s->info).c=NULL;
	(rtsp_s->info).b=NULL;
	(rtsp_s->info).t=NULL;
	(rtsp_s->info).r=NULL;
	(rtsp_s->info).z=NULL;
	(rtsp_s->info).k=NULL;
	(rtsp_s->info).attr_list=NULL;

	return rtsp_s;
}
