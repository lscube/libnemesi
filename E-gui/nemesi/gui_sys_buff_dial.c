/* * 
 *  ./E-gui/nemesi/gui_buff_dial.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:05 $
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

#include <sys/time.h>
#include <stdio.h>

#include "support.h"
#include <ourhdr.h>
#include <nemesi/decoder.h>

gint sys_dial_timeout(gpointer);

int gui_sys_buff_dial(void)
{
	static gint timer=-1;
	GtkDial *dial;

	dial=GTK_DIAL( lookup_widget(get_egui(NULL), "dial2") );

	if (timer > 0)
		gtk_timeout_remove(timer);
	timer=gtk_timeout_add(50, sys_dial_timeout, (gpointer *) dial);

	return 0;
}

gint sys_dial_timeout(gpointer data)
{
	gfloat new_val;
	struct RTSP_args *rtsp_args;

	rtsp_args=get_rtsp_args(NULL);

	if (rtsp_args->rtsp_th->status == INIT) {
		gtk_dial_set_percentage( GTK_DIAL(data), 0 );
		return FALSE;/* si rimuove il timeout */
	}

	new_val=sys_buff_size(NULL);
	/*fprintf(stderr, "%4.1f", new_val);*/
	/*new_val = ((float)(rtsp_args->rtsp_th->rtsp_queue->media_queue->rtp_sess->bp).flcount/(float)BP_SLOT_NUM);*/
	new_val=gtk_dial_set_percentage(GTK_DIAL(data), new_val);

	return TRUE;
}

