/* * 
 *  $Id:rtcp_schedule.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtcp.h>

struct rtcp_event *rtcp_schedule(struct rtcp_event *head, struct rtp_session *rtp_sess, struct timeval tv, rtcp_type_t type)
{
	struct rtcp_event *new_event;
	struct rtcp_event *pevent=head;
	struct rtcp_event *event=head;
	
	if( (new_event=(struct rtcp_event *)malloc(sizeof(struct rtcp_event))) == NULL) {
		nms_printf(NMSML_FATAL, "Cannot allocate memory!\n");
		return NULL;
	}
	new_event->rtp_sess=rtp_sess;
	new_event->tv=tv;
	new_event->type=type;
	new_event->next=NULL;

	if ( ! head )
		return new_event;

	while( event && timeval_subtract(NULL, &(event->tv), &tv)){
		pevent=event;
		event=event->next;
	}
	if (pevent == head ){
		new_event->next=head;
		return new_event;
	}
	pevent->next=new_event;
	new_event->next=event;
	
	return head;
}
