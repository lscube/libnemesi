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

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#include <ourhdr.h>
#include <nemesi/etui.h>


void
on_new_file1_activate                  (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_open1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_save_as1_activate                   (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_exit1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_cut1_activate                       (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_copy1_activate                      (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_paste1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_clear1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}


void
on_properties1_activate                (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{

}

void
on_preferences1_activate               (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *font_dialog;

	font_dialog = create_fontselectiondialog1();
	gtk_widget_show(font_dialog);
}

void
on_about1_activate                     (GtkMenuItem     *menuitem,
                                        gpointer         user_data)
{
	GtkWidget *about;
	
	about = create_about2();
	gtk_widget_show(about);

}

void
on_button3_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;

	rtsp_args=get_rtsp_args(NULL);
	rtsp_args->rtsp_th->busy=1;
	gui_throbber();
}

void
on_button4_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;

	rtsp_args=get_rtsp_args(NULL);
	rtsp_args->rtsp_th->busy=0;
}

void
on_button5_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
}

void
on_button7_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *open_dialog;
	
	open_dialog = create_open_dialog();
	gtk_widget_show(open_dialog);
}

void
on_button8_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;

	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
	rtsp_args=get_rtsp_args(NULL);
	send_close(rtsp_args);
	gui_throbber();
}

void
on_button9_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;
	char argstr[]="0";
	
	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
	rtsp_args=get_rtsp_args(NULL);
	send_play(rtsp_args, argstr);
	uiprintf("PLAY request sent ...\n");
	gui_throbber();
}

void
on_button10_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;

	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
	rtsp_args=get_rtsp_args(NULL);
	send_pause(rtsp_args, 'z');
	uiprintf("PAUSE request sent...\n");
	gui_throbber();
}

void
on_button11_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;

	gtk_widget_set_sensitive(GTK_WIDGET(user_data), FALSE);
	rtsp_args=get_rtsp_args(NULL);
	send_pause(rtsp_args, 's');
	uiprintf("STOP request sent...\n");
	gui_throbber();
}

void
on_button14_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;
	gchar *urlname = NULL;

	rtsp_args=get_rtsp_args(NULL);
	if (((urlname = gtk_entry_get_text(GTK_ENTRY(GTK_COMBO(user_data)->entry)))!=NULL) \
			&& (strlen(urlname)>0)) {
		gtk_widget_set_sensitive(lookup_widget(get_egui(NULL), "toolbar2"), FALSE);
		uiprintf("Connect: please wait, opening \"%s\"\n", urlname);
		send_open(rtsp_args, urlname);
		gui_throbber();
	}
}


void
on_button17_clicked                    (GtkButton       *button,
                                        gpointer         user_data)
{
	struct RTSP_args *rtsp_args;

	rtsp_args=get_rtsp_args(NULL);
	get_infos(rtsp_args);
}


void
on_apply_button1_clicked               (GtkButton       *button,
                                        gpointer         user_data)
{
	GdkFont *font;
	GtkStyle *style, *new_style;
	GtkWidget *text;

	text=lookup_widget(get_egui(NULL), "text1");
	font=gtk_font_selection_dialog_get_font((GtkFontSelectionDialog *) user_data);
	if (font == NULL) {
		fprintf(stderr,"Font %s not present...\n",\
				gtk_font_selection_dialog_get_font_name(\
					(GtkFontSelectionDialog *) user_data));
		return;
	}
	style=gtk_widget_get_style(text);
	style->font=font;
	new_style=gtk_style_copy(style);
	gtk_widget_set_style(text, new_style);
}


void
on_ok_button1_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
	GdkFont *font;
	GtkStyle *style, *new_style;
	GtkWidget *text;

	text=lookup_widget(get_egui(NULL), "text1");
	font=gtk_font_selection_dialog_get_font((GtkFontSelectionDialog *) user_data);
	if (font == NULL) {
		fprintf(stderr,"Font %s not present...\n",\
				gtk_font_selection_dialog_get_font_name(\
					(GtkFontSelectionDialog *) user_data));
		return;
	}
	style=gtk_widget_get_style(text);
	style->font=font;
	new_style=gtk_style_copy(style);
	gtk_widget_set_style(text, new_style);
	gtk_widget_destroy(GTK_WIDGET(user_data));

}

