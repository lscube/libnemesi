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

#include <sys/time.h>

#include "support.h"
#include <ourhdr.h>

gint progress_timeout(gpointer);

int gui_throbber(void)
{
	static gint timer=-1;
	GtkProgress *progress;

	progress=GTK_PROGRESS(((GnomeAppBar *)lookup_widget(get_egui(NULL),\
					"appbar1"))->progress);

	gtk_progress_set_activity_mode(progress, 1);
	if (timer > 0)
		gtk_timeout_remove(timer);
	timer=gtk_timeout_add(10, progress_timeout, (gpointer *) progress);

	return 0;
}

gint progress_timeout(gpointer data)
{
	gfloat new_val;
	GtkAdjustment *adj;
	struct RTSP_args *rtsp_args;

	new_val=gtk_progress_get_value( GTK_PROGRESS(data)) + 1;

	adj = GTK_PROGRESS (data)->adjustment;
	if (new_val > adj->upper)
		new_val = adj->lower;
	
	gtk_progress_set_value(GTK_PROGRESS(data), new_val);
	
	rtsp_args=get_rtsp_args(NULL);
	if (rtsp_args->rtsp_th->busy == 0) {
		update_toolbar(rtsp_args->rtsp_th);
		gtk_progress_set_activity_mode(GTK_PROGRESS(data), 0);
		gtk_progress_set_value(GTK_PROGRESS(data), adj->lower);
		uiprintf("Done.\n");
		return FALSE;/* si rimuove il timeout */
	}

	return TRUE;
}
