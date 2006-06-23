/* * 
 *  $Id:set_rtsp_media.c 267 2006-01-12 17:19:45Z shawill $
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
#include <nemesi/methods.h>
#include <nemesi/utils.h>
#include <nemesi/rtpptdefs.h>

int set_rtsp_media(struct rtsp_thread *rtsp_th)
{

	struct rtsp_session *curr_rtsp_s=rtsp_th->rtsp_queue;
	struct rtsp_medium *curr_rtsp_m=NULL;
	sdp_medium_info *sdp_m;
	sdp_attr *sdp_attr;
	char *tkn, *ch;
	uint8 pt;
	rtp_pt *rtppt;
	rtp_fmts_list *fmt, **prev_fmt;

	switch (rtsp_th->descr_fmt) {
		case DESCRIPTION_SDP_FORMAT :
			for(sdp_m=curr_rtsp_s->info->media_info_queue; sdp_m; sdp_m=sdp_m->next) {
				if ( curr_rtsp_m==NULL ) {
					/* first medium */
					if ( ( curr_rtsp_s->media_queue=curr_rtsp_m=rtsp_med_create(rtsp_th->fd) )==NULL )
						return 1;
				} else if ( rtsp_th->type==CONTAINER ) {
					/* media in the same session */
					if ( (curr_rtsp_m->next=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_m->next->rtp_sess;
					curr_rtsp_m=curr_rtsp_m->next;
				} else if ( rtsp_th->type==M_ON_DEMAND ) {
					/* one medium for each session */
					if ( (curr_rtsp_s->next=rtsp_sess_dup(curr_rtsp_s))==NULL )
						return 1;
					curr_rtsp_s=curr_rtsp_s->next;
					if ( (curr_rtsp_s->media_queue=rtsp_med_create(rtsp_th->fd))==NULL )
						return 1;
					curr_rtsp_m->rtp_sess->next=curr_rtsp_s->media_queue->rtp_sess;
					curr_rtsp_m=curr_rtsp_s->media_queue;
				}
				curr_rtsp_m->medium_info = sdp_m;
				
				// setup rtp format list for current media
				for (tkn=sdp_m->fmts, prev_fmt=&curr_rtsp_m->fmts; *tkn && (!(pt = strtoul(tkn, &ch, 10)) && ch==tkn); tkn=ch) {
					if (pt>127) {
						nms_printf(NMSML_ERR, "sdp 'm=' field specified an rtp payload type not valid (%u)\n", pt);
						return 1;
					} else if (pt >= 96) {
						switch (sdp_m->media_type) {
							case 'A':
								if ( !(rtppt = rtp_pt_new(AU)) )
									return 1;
								break;
							case 'V':
								if ( !(rtppt = rtp_pt_new(VI)) )
									return 1;
								break;
							default:
								// not recognized
								if ( !(rtppt = rtp_pt_new(NA)) )
									return 1;
								break;
						}
						rtp_dynpt_set(curr_rtsp_m->rtp_sess->rtpptdefs, rtppt, pt);
					}
					if ( !(fmt=malloc(sizeof(rtp_fmts_list))) ) {
						nms_printf(NMSML_FATAL, "Could not alloc memory for rtp_fmts_list\n");
						return 1;
					}
					fmt->rtppt = curr_rtsp_m->rtp_sess->rtpptdefs[pt];
					fmt->next = NULL;
					*prev_fmt = fmt;
					prev_fmt = &fmt->next;
				}
				
				for(sdp_attr=sdp_m->attr_list; sdp_attr; sdp_attr=sdp_attr->next) {
					if ( !strncmpcase(sdp_attr->a, "control", 7) ) {
						tkn = sdp_attr->a + 7; // 7 == strlen("control")
						while ( (*tkn==' ') || (*tkn==':') ) // skip spaces and colon
							tkn++;
						curr_rtsp_m->filename=tkn;
					}
					if ( !strncmpcase(sdp_attr->a, "rtpmap", 6) ) {
						/* We assume the string in the format:
					 	* rtpmap:PaloadType EncodingName/ClockRate[/Channels] */
						tkn = sdp_attr->a + 6; // 6 == strlen("rtpmap")
						// skip spaces and colon (we should not do this!)
						while ( (*tkn==' ') || (*tkn==':') )
							tkn++;
						if ( ((pt=(uint8)strtoul(tkn, &tkn, 10)) >= 96) && ( pt <= 127 ) ) {
							while ( *tkn == ' ' )
								tkn++;
							if ( !(ch=strchr(tkn, '/')) ){
								nms_printf(NMSML_WARN, "Invalid field rtpmap.\n");
								break;
							}
							*ch = '\0';
							switch (sdp_m->media_type) {
								if ( rtp_dynpt_encname(curr_rtsp_m->rtp_sess->rtpptdefs, pt, tkn) )
									return 1;
								case 'A':
									sscanf(ch+1, "%u/%c", &curr_rtsp_m->rtp_sess->rtpptdefs[pt]->rate, &RTP_AUDIO(curr_rtsp_m->rtp_sess->rtpptdefs[pt])->channels);
									break;
								case 'V':
									sscanf(ch+1, "%u", &curr_rtsp_m->rtp_sess->rtpptdefs[pt]->rate);
									break;
								default:
									// not recognized
									break;
							}
							*ch='/';
							tkn=++ch;
						} else {
							// shawill: should be an error or a warning?
							nms_printf(NMSML_WARN, "Warning: rtpmap attribute is trying to set a non-dynamic payload type: not permitted\n");
						}
					}
					if ( !strncmpcase(sdp_attr->a, "fmtp", 4) ) {
						/* We assume the string in the format:
					 	* fmtp:PaloadType <format specific parameters> */
						tkn = sdp_attr->a + 4; // 4 == strlen("fmtp")
						// skip spaces and colon (we should not do this!)
						while ( (*tkn==' ') || (*tkn==':') )
							tkn++;
						if ( (pt=(uint8)strtoul(tkn, &tkn, 10)) <= 127 ) {
							while ( *tkn == ' ' )
								tkn++;
							rtp_pt_attr_add(curr_rtsp_m->rtp_sess->rtpptdefs, pt, tkn);
						} else {
							// shawill: should be an error or a warning?
							nms_printf(NMSML_WARN, "Warning: fmtp attribute is trying to set an out of bounds payload type: not permitted\n");
						}
					}
				}
			}
			break;
		case DESCRIPTION_MH_FORMAT :
			/* not yet implemented */
			// break;
		default :
			nms_printf(NMSML_ERR, "Unknown decription format.\n");
			return 1;
			break;
	}

	return 0;
}
