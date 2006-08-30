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

#include <nemesi/cc.h>
#include <nemesi/methods.h>

#include "support.h"
#include "gui_statusbar.h"
#include "gui_cc.h"
#include "callbacks.h"

// #define CC_SOMERIGHTS "somerights.png"
#define CC_SOMERIGHTS "CreativeCommonsSomeRights2.png"

static GtkWidget *cc_box = NULL;
static GtkWidget *cc_button = NULL;
static GtkWidget *cc_logo;

static GtkWidget *cc_info = NULL;

static GtkWidget *cc_pix[5] = { NULL, NULL, NULL, NULL, NULL};

static void cc_box_add_pixmap(char *pixname)
{
	const int max_pixs = sizeof(cc_pix)/sizeof(GtkWidget *);
	int pix_idx;

	for (pix_idx=0; pix_idx<max_pixs && cc_pix[pix_idx]; pix_idx++);
	if (pix_idx >= max_pixs)
		return;

	cc_pix[pix_idx] = create_pixmap(NULL, pixname);
	gtk_widget_show(cc_pix[pix_idx]);
	gtk_container_add (GTK_CONTAINER (cc_box), cc_pix[pix_idx]);
	// gtk_widget_show (cc_button);
	
	pix_idx++;

	return;
}

static void cc_box_clear_pixmaps(void)
{
	const int max_pixs = sizeof(cc_pix)/sizeof(GtkWidget *);
	int pix_idx;

	for (pix_idx=0; pix_idx<max_pixs && cc_pix[pix_idx]; pix_idx++) {
		gtk_widget_destroy(cc_pix[pix_idx]);
		cc_pix[pix_idx] = NULL;
	}
}

static GtkWidget *add_root(GtkWidget **root)
{
	GtkWidget *expndr, *box;

	expndr = gtk_expander_new("session infos");
	if (root)
		*root = expndr;

	box = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box);
	gtk_container_add (GTK_CONTAINER(expndr), box);

	gtk_expander_set_expanded (GTK_EXPANDER(expndr), TRUE);

	gtk_widget_show(expndr);

	return box;
}

static GtkWidget *add_node(const GtkWidget *where, const gchar *label)
{
	GtkWidget *expndr, *box, *fixed;

	fixed = gtk_fixed_new();
	gtk_widget_show(fixed);
	if (where)
		gtk_box_pack_start (GTK_BOX (where), fixed, FALSE, FALSE, 0);

	expndr = gtk_expander_new(label);
	gtk_expander_set_expanded (GTK_EXPANDER(expndr), TRUE);
	gtk_widget_show(expndr);
	gtk_fixed_put(GTK_FIXED(fixed), expndr, 16, 0);

	box = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box);
	gtk_container_add (GTK_CONTAINER(expndr), box);

	gtk_widget_show(fixed);

	return box;
}

static GtkWidget *add_leaf(const GtkWidget *where)
{
	GtkWidget *box, *fixed;

	fixed = gtk_fixed_new();
	gtk_widget_show(fixed);
	if (where)
		gtk_box_pack_start (GTK_BOX (where), fixed, FALSE, FALSE, 0);

	box = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(box);
	gtk_fixed_put(GTK_FIXED(fixed), box, 16, 0);

	gtk_widget_show(fixed);

	return box;
}

static void cc_button_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
	rtsp_ctrl *rtsp_ctl = (rtsp_ctrl *)user_data;
	sdp_session_info *sdp_s;
	sdp_medium_info *sdp_m;
	cc_license *cc;
	GtkWidget *root, *node, *leaf, *lbl;

	if (gtk_toggle_button_get_active(togglebutton)) {
		if (!rtsp_ctl->rtsp_queue)
			return;
		if (!cc_info) {
			root = add_root(&cc_info);

			switch (rtsp_ctl->descr_fmt) {
				case DESCRIPTION_SDP_FORMAT:
					if ((sdp_s = rtsp_ctl->rtsp_queue->info)) {
						gtk_expander_set_label(GTK_EXPANDER(cc_info), sdp_s->s);
					}
					for(sdp_m=sdp_s->media_info_queue; sdp_m; sdp_m=sdp_m->next) {
						node = add_node(root, sdp_m->m);
						cc = sdp_m->cc;
						if (cc->uriLicense) {
							leaf = add_leaf(node);
							lbl = gtk_label_new(cc->uriLicense);
							gtk_box_pack_start (GTK_BOX (leaf), lbl, FALSE, FALSE, 0);
							gtk_widget_show(lbl);
						}
						if (cc->uriMetadata) {
							leaf = add_leaf(node);
							lbl = gtk_label_new(cc->uriMetadata);
							gtk_box_pack_start (GTK_BOX (leaf), lbl, FALSE, FALSE, 0);
							gtk_widget_show(lbl);
						}
						if (cc->title) {
							leaf = add_leaf(node);
							lbl = gtk_label_new(cc->title);
							gtk_box_pack_start (GTK_BOX (leaf), lbl, FALSE, FALSE, 0);
							gtk_widget_show(lbl);
						}
						if (cc->creator) {
							leaf = add_leaf(node);
							lbl = gtk_label_new(cc->creator);
							gtk_box_pack_start (GTK_BOX (leaf), lbl, FALSE, FALSE, 0);
							gtk_widget_show(lbl);
						}
					}
					break;
				default:
					break;
			}
		}
		gtk_widget_show(cc_info);
		view_info(cc_info);
	} else {
		if (cc_info) {
			gtk_widget_destroy(cc_info);
			cc_info = NULL;
		}
		hide_info();
	}
}

static gboolean cc_sdp_check(sdp_medium_info *sdp_mqueue)
{
	sdp_medium_info *sdp_m;
	gboolean iscc=FALSE;
	CC_BITMASK_T msk1st, mskcur;
	cc_perm_mask *msk=(cc_perm_mask *)&msk1st;

	cc_box_clear_pixmaps();
	for (sdp_m=sdp_mqueue; sdp_m; sdp_m=sdp_m->next)
		if(sdp_m->cc) {
			if (!iscc)
				cc_parse_urilicense(sdp_m->cc->uriLicense, (cc_perm_mask *)(&msk1st));
			else {
				cc_parse_urilicense(sdp_m->cc->uriLicense, (cc_perm_mask *)(&mskcur));
				if(msk1st ^ mskcur) {
					iscc=TRUE;
					msk=NULL;
					break;
				}
			}
			iscc=TRUE;
		}

	if (msk) {
		if (msk->spec_license)
			cc_box_add_pixmap("pd.png"); // TODO: aggiustare
		else {
			if (msk->by)
				cc_box_add_pixmap("by.png");
			if (msk->nc)
				cc_box_add_pixmap("nc.png");
			if (msk->nd)
				cc_box_add_pixmap("nd.png");
			if (msk->sa)
				cc_box_add_pixmap("sa.png");
		}
	}

	return iscc;
}

static void cc_stbarw_upd(void *userdata)
{
	rtsp_ctrl *rtsp_ctl = (rtsp_ctrl *)userdata;

	switch (rtsp_status(rtsp_ctl)) {
		case READY:
		case PLAYING:
			switch (rtsp_ctl->descr_fmt) {
				case DESCRIPTION_SDP_FORMAT:
					if (cc_sdp_check(rtsp_ctl->rtsp_queue->media_queue->medium_info))
						gtk_widget_show(cc_box);
					break;
				default:
					break;
			}

			break;
		case INIT:
		default:
			if (cc_info) {
				gtk_widget_destroy(cc_info);
				cc_info = NULL;
			}
			gtk_widget_hide(cc_box);
			break;
	}
	
	return;
}

static void cc_stbarw_rm(GtkWidget *widget)
{
	if (cc_info)
		gtk_widget_destroy(cc_info);

	gtk_widget_destroy(widget);
}

int cc_stbarw_add(rtsp_ctrl *rtsp_ctl)
{
	if (cc_box)
		gtk_widget_destroy(cc_box);

	cc_box = gtk_hbox_new(FALSE, 0);

	cc_button = gtk_toggle_button_new();
	gtk_widget_show(cc_button);
	gtk_box_pack_start (GTK_BOX (cc_box), cc_button, FALSE, FALSE, 0);

	cc_logo = create_pixmap(NULL, CC_SOMERIGHTS);
	gtk_widget_show(cc_logo);
	gtk_container_add (GTK_CONTAINER (cc_button), cc_logo);
	g_signal_connect ((gpointer) cc_button, "toggled", G_CALLBACK (cc_button_toggled), (gpointer) rtsp_ctl);
	// gtk_widget_show (cc_button);

	gnms_stbar_addwgt(cc_box, cc_stbarw_rm, cc_stbarw_upd, (gpointer)rtsp_ctl, TRUE);

	return 0;
}

