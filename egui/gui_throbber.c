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

#include <gtk/gtk.h>

#include <nemesi/egui.h>
#include <nemesi/comm.h>

// static GtkProgress *progress;

static gint progress_timeout(gpointer data);

int gui_throbber(struct RTSP_args *rtsp_args)
{
	static gint timer=-1;

	if (timer > 0)
		g_source_remove(timer);
	timer = g_timeout_add(10, progress_timeout, (gpointer *) rtsp_args);
	return 0;
}

static gint progress_timeout(gpointer data)
{
	struct RTSP_args *rtsp_args = data;

	if (!rtsp_args->rtsp_th->busy) {
		nmsprintf(1, "Done\n");
		return FALSE;
	}
	return TRUE;
}

