/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gnmsprint.h"
#include "gui_statusbar.h"

#define MAX_STRLEN	1024
#define MAX_MSG		4096

typedef struct {
	GtkWidget *window;
	GtkLabel *label;
	GtkImage *error;
	GtkImage *warning;
} GNMSdialog;

static struct {
	int level;
	char msg[MAX_MSG];
} gmsg = {NMSML_NORM, ""};

static int gnmsprintf(int level, const char *fmt, ...);
static int gnmsstatusprintf(int cmd, const char *fmt, ...);
static GNMSdialog* gnmsprintf_create(void);
static void gnmsprintf_destroy(GtkButton *button, gpointer user_data);

void gnmsprint_init(void)
{
	nmsprintf = gnmsprintf;
	nmsstatusprintf = gnmsstatusprintf;
}

int gnms_showmsgs()
{
	static GNMSdialog *dialog = NULL;

	if ( *gmsg.msg ) { // there is a message
		if ( (!dialog) && !(dialog = gnmsprintf_create()) )
			return nmsprintf_default(NMSML_FATAL, "Could not initilize GTK dialog\n");
		
		gtk_label_set_text(dialog->label, gmsg.msg);
		switch (gmsg.level) {
			case NMSML_FATAL:
				gtk_window_set_title (GTK_WINDOW (dialog->window), "NeMeSI - FATAL");
			case NMSML_ERR:
				gtk_window_set_title (GTK_WINDOW (dialog->window), "NeMeSI - ERROR");
				gtk_widget_hide(GTK_WIDGET(dialog->warning));
				gtk_widget_show(GTK_WIDGET(dialog->error));
				break;
			case NMSML_WARN:
				gtk_window_set_title (GTK_WINDOW (dialog->window), "NeMeSI - WARNING");
				gtk_widget_hide(GTK_WIDGET(dialog->error));
				gtk_widget_show(GTK_WIDGET(dialog->warning));
				break;
			default:
				gtk_window_set_title (GTK_WINDOW (dialog->window), "NeMeSI");
				gtk_widget_hide(GTK_WIDGET(dialog->error));
				gtk_widget_hide(GTK_WIDGET(dialog->warning));
				break;
		}
		gtk_widget_show(dialog->window);

		*gmsg.msg = '\0';
		gmsg.level = NMSML_NORM;
	}

	return 0;
}

static int gnmsprintf(int level, const char *fmt, ...)
{
	va_list args;
	char tmp[MAX_STRLEN];

	va_start(args, fmt);
	vsnprintf(tmp, sizeof(tmp), fmt, args);
	va_end(args);
	tmp[sizeof(tmp)-2] = '\n';
	tmp[sizeof(tmp)-1] = '\0';

	switch (level) {
		case NMSML_FATAL:
			gmsg.level = NMSML_FATAL;
		case NMSML_ERR:
			if (gmsg.level > NMSML_ERR)
				gmsg.level = NMSML_ERR;
		case NMSML_WARN:
			if (gmsg.level > NMSML_WARN)
				gmsg.level = NMSML_WARN;
			strncat(gmsg.msg+strlen(gmsg.msg), tmp, sizeof(gmsg.msg)-strlen(gmsg.msg));
			gmsg.msg[sizeof(gmsg.msg)-1] = '\0';
			break;
		case NMSML_ALWAYS:
			gnms_stbar_setstr("%s", tmp);
			break;
		default:
			nmsprintf_default(level, "%s", tmp);
			break;
	}
	
	return ( level < NMSML_WARN ) ? 1 : 0;
}

static int gnmsstatusprintf(int cmd, const char *fmt, ...)
{
	// TODO: implement.
	return 0;
}

static GNMSdialog* gnmsprintf_create(void)
{
	GNMSdialog *dialog;
	GtkWidget *gnmsprintf;
	GtkWidget *dialog_vbox;
	GtkWidget *hbox;
	GtkWidget *image1;
	GtkWidget *image2;
	GtkWidget *gnmsprintf_l;
	GtkWidget *dialog_action_area;
	GtkWidget *closegnmsprint;

	if ( !(dialog=malloc(sizeof(GNMSdialog))) )
		return NULL;
	
	gnmsprintf = gtk_dialog_new ();
	// gtk_widget_set_name (gnmsprintf, "gnmsprintf");
	gtk_window_set_title (GTK_WINDOW (gnmsprintf), "NeMeSI");
	gtk_window_set_position (GTK_WINDOW (gnmsprintf), GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_window_set_modal (GTK_WINDOW (gnmsprintf), TRUE);
	gtk_window_set_type_hint (GTK_WINDOW (gnmsprintf), GDK_WINDOW_TYPE_HINT_DIALOG);
	
	dialog_vbox = GTK_DIALOG (gnmsprintf)->vbox;
	// gtk_widget_set_name (dialog_vbox, "dialog_vbox2");
	gtk_widget_show (dialog_vbox);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_widget_show (hbox);
	gtk_box_pack_start (GTK_BOX (dialog_vbox), hbox, TRUE, TRUE, 0);

	image1 = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_DIALOG);
	// image1 = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_DND);
	// image1 = gtk_image_new_from_stock ("gtk-dialog-error", GTK_ICON_SIZE_BUTTON);
	// gtk_widget_show (image1);
	gtk_box_pack_start (GTK_BOX (hbox), image1, TRUE, TRUE, 0);
	
	image2 = gtk_image_new_from_stock ("gtk-dialog-warning", GTK_ICON_SIZE_DIALOG);
	// image2 = gtk_image_new_from_stock ("gtk-dialog-warning", GTK_ICON_SIZE_DND);
	// image2 = gtk_image_new_from_stock ("gtk-dialog-warning", GTK_ICON_SIZE_BUTTON);
	// gtk_widget_show (image2);
	gtk_box_pack_start (GTK_BOX (hbox), image2, TRUE, TRUE, 0);
	
	gnmsprintf_l = gtk_label_new (NULL);
	// gtk_widget_set_name (gnmsprintf_l, "gnmsprintf_l");
	gtk_widget_show (gnmsprintf_l);
	gtk_box_pack_start (GTK_BOX (hbox), gnmsprintf_l, TRUE, TRUE, 0);
	gtk_label_set_justify (GTK_LABEL (gnmsprintf_l), GTK_JUSTIFY_LEFT);
	
	dialog_action_area = GTK_DIALOG (gnmsprintf)->action_area;
	// gtk_widget_set_name (dialog_action_area, "dialog_action_area3");
	gtk_widget_show (dialog_action_area);
	gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area), GTK_BUTTONBOX_END);
	
	closegnmsprint = gtk_button_new_from_stock ("gtk-close");
	// gtk_widget_set_name (closegnmsprint, "closegnmsprint");
	gtk_widget_show (closegnmsprint);
	gtk_dialog_add_action_widget (GTK_DIALOG (gnmsprintf), closegnmsprint, GTK_RESPONSE_CLOSE);
	GTK_WIDGET_SET_FLAGS (closegnmsprint, GTK_CAN_DEFAULT);
	
	g_signal_connect((gpointer) closegnmsprint, "clicked", G_CALLBACK (gnmsprintf_destroy), gnmsprintf);

	dialog->window = gnmsprintf;
	dialog->label = GTK_LABEL(gnmsprintf_l);
	dialog->error = GTK_IMAGE(image1);
	dialog->warning = GTK_IMAGE(image2);
	
	return dialog;
}


static void gnmsprintf_destroy(GtkButton *button, gpointer user_data)
{
	gtk_widget_hide(GTK_WIDGET(user_data));
	// gtk_widget_destroy(GTK_WIDGET(user_data));
}

