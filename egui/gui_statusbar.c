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

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "support.h"

#include "gui_statusbar.h"

#define MAXSTATUSSTR 512

typedef struct _gnms_stwidget {
	GtkWidget *widget;
	void (*updater) (void *userdata);
	gpointer updata;
	void (*destroyer) (GtkWidget *);
	struct _gnms_stwidget *next;
} GNMSstWidget;

typedef struct {
	GtkBox *statusbox;
	GtkStatusbar *statusbar;
	guint status_cid;
	GNMSstWidget *widgetq;
} GNMSStatusBar;

static GNMSStatusBar nmsstatusbar;

void gnms_stbar_init(GtkBox * statusbox)
{
	nmsstatusbar.statusbox = statusbox;
	nmsstatusbar.statusbar =
	    GTK_STATUSBAR(lookup_widget(GTK_WIDGET(statusbox), "statusbar"));
	nmsstatusbar.status_cid =
	    gtk_statusbar_get_context_id(nmsstatusbar.statusbar, "Status");
	nmsstatusbar.widgetq = NULL;
	return;
}

int gnms_stbar_setstr(const char *fmt, ...)
{
	char statusstr[MAXSTATUSSTR];
	int ret;
	va_list args;

	va_start(args, fmt);
	ret = vsnprintf(statusstr, sizeof(statusstr), fmt, args);
	va_end(args);

	statusstr[sizeof(statusstr) - 1] = '\0';

	gtk_statusbar_push(nmsstatusbar.statusbar, nmsstatusbar.status_cid,
			   statusstr);

	return ret;
}

static GNMSstWidget *stbar_stw_new(void)
{
	GNMSstWidget *newstw;

	if (!(newstw = malloc(sizeof(GNMSstWidget))))
		return NULL;

	newstw->updater = NULL;
	newstw->updata = NULL;
	newstw->destroyer = NULL;

	// insert on head of widget list
	newstw->next = nmsstatusbar.widgetq;
	nmsstatusbar.widgetq = newstw;

	return newstw;
}

static void stbar_add_separator()
{
	GNMSstWidget *newstw;

	if (!(newstw = stbar_stw_new()))
		return;

	newstw->widget = gtk_hseparator_new();

	gtk_box_pack_end(nmsstatusbar.statusbox, newstw->widget, FALSE, FALSE,
			 0);
	gtk_widget_show(newstw->widget);

	return;
}

int gnms_stbar_addwgt(GtkWidget * widget, void (*destroyer) (GtkWidget *),
		      void (*updater) (void *), gpointer updata,
		      gboolean separator)
{
	GNMSstWidget *newstw;

	if (separator)
		stbar_add_separator();

	if (!(newstw = stbar_stw_new()))
		return 1;

	newstw->widget = widget;

	// widget updaters
	newstw->updater = updater;
	newstw->updata = updata;
	// widget destroyers
	newstw->destroyer = destroyer;

	// packing
	gtk_box_pack_end(nmsstatusbar.statusbox, newstw->widget, FALSE, FALSE,
			 0);

	// show widgets
	gtk_widget_show(newstw->widget);

	return 0;
}

void gnms_stbar_update(void)
{
	GNMSstWidget *stw;

	for (stw = nmsstatusbar.widgetq; stw; stw = stw->next) {
		if (stw->updater)
			stw->updater(stw->updata);
	}
	return;
}

/*
void gnms_stbar_rmwgt(GtkWidget *widget)
{
	return;
}
*/

void gnms_stbar_clear(void)
{
	GNMSstWidget *stw;

	for (stw = nmsstatusbar.widgetq; stw; stw = stw->next) {
		if (stw->destroyer)
			stw->destroyer(stw->widget);
		else
			gtk_widget_destroy(stw->widget);
		free(stw);
	}

	nmsstatusbar.widgetq = NULL;


	return;
}
