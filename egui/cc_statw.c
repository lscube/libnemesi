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

// #define CC_SOMERIGHTS "somerights.png"
#define CC_SOMERIGHTS "CreativeCommonsSomeRights2.png"

static GtkWidget *cc_box = NULL;
static GtkWidget *cc_button = NULL;
static GtkWidget *cc_logo;

static GtkWidget *cc_pix[5] = { NULL, NULL, NULL, NULL, NULL};

static void cc_box_add_pixmap(char *pixname)
{
	static int pix_index = 0;

	cc_pix[pix_index] = create_pixmap(NULL, pixname);
	gtk_widget_show(cc_pix[pix_index]);
	gtk_container_add (GTK_CONTAINER (cc_box), cc_pix[pix_index]);
	// gtk_widget_show (cc_button);
	
	pix_index++;

	return;
}

static gboolean cc_sdp_check(SDP_Medium_info *sdp_mqueue)
{
	SDP_Medium_info *sdp_m;
	gboolean iscc=FALSE;
	CC_BITMASK_T msk1st, mskcur;
	CCPermsMask *msk=(CCPermsMask *)&msk1st;

	for (sdp_m=sdp_mqueue; sdp_m; sdp_m=sdp_m->next)
		if(sdp_m->cc) {
			if (!iscc)
				cc_parse_urilicense(sdp_m->cc->uriLicense, (CCPermsMask *)(&msk1st));
			else {
				cc_parse_urilicense(sdp_m->cc->uriLicense, (CCPermsMask *)(&mskcur));
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
	struct RTSP_Thread *rtsp_th = (struct RTSP_Thread *)userdata;

	switch (rtsp_th->status) {
		case READY:
		case PLAYING:
			// cc_box_add_pixmap("nd.png");
			// cc_box_add_pixmap("sa.png");
			switch (rtsp_th->descr_fmt) {
				case DESCRIPTION_SDP_FORMAT:
					if (cc_sdp_check(rtsp_th->rtsp_queue->media_queue->medium_info))
						gtk_widget_show(cc_box);
					break;
				default:
					break;
			}

			break;
		case INIT:
		default:
			gtk_widget_hide(cc_box);
			break;
	}
	
	return;
}

static void cc_stbarw_rm(GtkWidget *widget)
{
	gtk_widget_destroy(widget);
}

int cc_stbarw_add(struct RTSP_Thread *rtsp_th)
{
	if (cc_box)
		gtk_widget_destroy(cc_box);

	cc_box = gtk_hbox_new(FALSE, 0);

	cc_button = gtk_button_new();
	gtk_widget_show(cc_button);
	gtk_box_pack_start (GTK_BOX (cc_box), cc_button, FALSE, FALSE, 0);

	cc_logo = create_pixmap(NULL, CC_SOMERIGHTS);
	gtk_widget_show(cc_logo);
	gtk_container_add (GTK_CONTAINER (cc_button), cc_logo);
	// gtk_widget_show (cc_button);

	gnms_stbar_addwgt(cc_box, cc_stbarw_rm, cc_stbarw_upd, (gpointer)rtsp_th, TRUE);

	return 0;
}

