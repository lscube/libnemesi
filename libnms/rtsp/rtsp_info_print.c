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

#include <nemesi/rtsp.h>

// shawill: TODO: modify for protocol abstraction
void rtsp_info_print(rtsp_ctrl * rtsp_ctl)
{
	// tmp
	rtsp_thread *rtsp_th = (rtsp_thread *) rtsp_ctl;
	rtsp_session *sess;
	rtsp_medium *med;
	char **str;
	// struct attr *attr;
	sdp_attr *attr;

	char *sdes[ /*13 */ ] = { SDP_SESSION_FIELDS };
	char *mdes[ /*5 */ ] = { SDP_MEDIA_FIELDS };

	sess = rtsp_th->rtsp_queue;

	nms_printf(NMSML_NORM, BLANK_LINE);

	if (!sess) {
		nms_printf(NMSML_NORM, "No Connection!\n\n");
		return;
	}

	while (sess) {
		med = sess->media_queue;
		nms_printf(NMSML_NORM, "---- RTSP Session Infos: %s ----\n",
			   sess->pathname);
		for (str = (char **) (sess->info);
		     str < (char **) &(sess->info->attr_list); str++)
			if (*str)
				nms_printf(NMSML_ALWAYS, "* %s: %s\n",
					   sdes[str - (char **) (sess->info)],
					   *str);
		for (attr = sess->info->attr_list; attr; attr = attr->next)
			nms_printf(NMSML_ALWAYS, "%s\n", attr->a);
		while (med) {
			nms_printf(NMSML_NORM,
				   "\n\t---- RTSP Medium Infos: %s ----\n",
				   med->filename);
			for (str = (char **) (med->medium_info);
			     str < (char **) &(med->medium_info->attr_list);
			     str++)
				if (*str)
					nms_printf(NMSML_ALWAYS, "\t* %s: %s\n",
						   mdes[str -
							(char **) (med->
								   medium_info)],
						   *str);
			for (attr = med->medium_info->attr_list; attr;
			     attr = attr->next)
				nms_printf(NMSML_ALWAYS, "\t* %s\n", attr->a);
			med = med->next;
		}
		sess = sess->next;
	}
	nms_printf(NMSML_ALWAYS, "\n");

}
