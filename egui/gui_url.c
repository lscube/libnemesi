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

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>

#include <nemesi/comm.h>

#include "gui_url.h"

GNMSurl *gnmsurl_init(void)
{
	GNMSurl *new;
	GtkListStore *list_store;
	GtkTreeIter iter;

	if (!(new=malloc(sizeof(GNMSurl)))) {
		nmsprintf(NMSML_FATAL, "Could not alloc memory for completion struct\n");
		return NULL;
	}

	list_store = gtk_list_store_new (1, G_TYPE_STRING);
	gtk_list_store_append (list_store, &iter);
	gtk_list_store_set(list_store, &iter, 0, "rtsp://", -1);

	new->model = GTK_TREE_MODEL(list_store);
	new->txt_col = 0;
		
	return new;
}

gboolean gnmscompletion_machfunc(GtkEntryCompletion *completion, const gchar *key, GtkTreeIter *iter, gpointer user_data)
{
	GNMSurl *gnmsurl = (GNMSurl *)user_data;
	gchar *str_data;
	gboolean match;
	
	nmsprintf(NMSML_DBG3, "key to comp %s\n", key);
	
	gtk_tree_model_get (gnmsurl->model, iter, 0, &str_data, -1);
	if (!str_data) {
		nmsprintf(NMSML_DBG3, "no string to match\n");
		return FALSE;
	}
	nmsprintf(NMSML_DBG3, "%s\n", str_data);
	if (strstr(str_data, key))
		match = TRUE;
	else
		match = FALSE;
	g_free(str_data);

	return match;
}

void gnmsurl_destroy(gpointer data)
{
	GNMSurl *gnmsurl = (GNMSurl *)data;

	free(gnmsurl);
}

