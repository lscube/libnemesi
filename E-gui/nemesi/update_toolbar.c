/* * 
 *  ./E-gui/nemesi/update_toolbar.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:05 $
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

#include <gnome.h>

#include "support.h"
#include <ourhdr.h>

int update_toolbar(struct RTSP_Thread *rtsp_th)
{
	GtkWidget *egui;
	GnomeAppBar *appbar;
	char statusstr[27];

	egui=get_egui(NULL);
	sprintf(statusstr,"NeMeSI status: %s", statustostr(rtsp_th->status));
	appbar=(GnomeAppBar *)lookup_widget(egui, "appbar1");
	gtk_label_set_text((GtkLabel *)appbar->status, statusstr);
	switch (rtsp_th->status)
	{
		case INIT:
			gtk_widget_hide(lookup_widget(egui, "button8"));
			gtk_widget_show(lookup_widget(egui, "button7"));
			gtk_widget_hide(lookup_widget(egui, "button10"));
			gtk_widget_show(lookup_widget(egui, "button9"));
			break;
		case READY:
			gtk_widget_hide(lookup_widget(egui, "button7"));
			gtk_widget_show(lookup_widget(egui, "button8"));
			gtk_widget_hide(lookup_widget(egui, "button10"));
			gtk_widget_show(lookup_widget(egui, "button9"));
			gui_buff_dial();
			gui_sys_buff_dial();
			break;
		case PLAYING:
			gtk_widget_hide(lookup_widget(egui, "button7"));
			gtk_widget_show(lookup_widget(egui, "button8"));
			gtk_widget_hide(lookup_widget(egui, "button9"));
			gtk_widget_show(lookup_widget(egui, "button10"));
			gui_play_throbber();
			/*
			gui_buff_dial();
			*/
			break;
		case RECORDING:
			gtk_widget_hide(lookup_widget(egui, "button7"));
			gtk_widget_show(lookup_widget(egui, "button8"));
			break;
	}
	gtk_widget_set_sensitive(lookup_widget(egui, "toolbar2"), TRUE);

	return 0;
}
