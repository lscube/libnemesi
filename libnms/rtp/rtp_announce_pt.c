/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
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

#include <nemesi/rtp.h>

int rtp_announce_pt(rtp_session *rtp_sess, unsigned pt, rtp_media_type media_type)
{
	rtp_pt *rtppt;
	rtp_fmts_list *fmt, **prev_fmt;
	
	if (pt>127) {
		nms_printf(NMSML_ERR, "rtp payload type not valid (%u)\n", pt);
		return RTP_ERROR;
	} else if (pt >= 96) {
		if ( !(rtppt = rtp_pt_new(media_type)) )
			return RTP_ERROR;
		rtp_dynpt_set(rtp_sess->rtpptdefs, rtppt, pt);
	}
	if ( !(fmt=malloc(sizeof(rtp_fmts_list))) ) {
		nms_printf(NMSML_FATAL, "Could not alloc memory for rtp_fmts_list\n");
		return RTP_ERRALLOC;
	}
	fmt->pt = pt;
	fmt->rtppt = rtp_sess->rtpptdefs[pt];
	fmt->next = NULL;
	for (prev_fmt=&rtp_sess->announced_fmts; *prev_fmt; prev_fmt = &(*prev_fmt)->next);
	*prev_fmt = fmt;
	prev_fmt = &fmt->next;
					
	return RTP_OK;
}
