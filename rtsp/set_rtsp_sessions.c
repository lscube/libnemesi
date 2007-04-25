/* * 
 *  $Id:set_rtsp_sessions.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
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

int set_rtsp_sessions(rtsp_thread * rtsp_th, int content_length,
              char *content_base, char *body)
{
    sdp_attr *sdp_a;
    char *tkn;

    switch (rtsp_th->descr_fmt) {
    case DESCRIPTION_SDP_FORMAT:
        if (!
            (rtsp_th->rtsp_queue =
             rtsp_sess_create(rtsp_th->urlname, content_base)))
            return 1;

        if (!
            (rtsp_th->rtsp_queue->body =
             (char *) malloc(content_length + 1)))
            return nms_printf(NMSML_FATAL,
                      "Cannot allocate memory.\n");
        memcpy(rtsp_th->rtsp_queue->body, body, content_length);
        rtsp_th->rtsp_queue->body[content_length] = '\0';

        rtsp_th->type = M_ON_DEMAND;

        if (!
            (rtsp_th->rtsp_queue->info =
             sdp_session_setup(rtsp_th->rtsp_queue->body,
                       content_length)))
            return nms_printf(NMSML_ERR, "SDP parse error\n");

        // we look for particular attributes of session
        for (sdp_a = rtsp_th->rtsp_queue->info->attr_list; sdp_a;
             sdp_a = sdp_a->next) {
            if (!strncmpcase(sdp_a->a, "control", 7)) {
                tkn = sdp_a->a + 7;    // 7 == strlen("control")
                while ((*tkn == ' ') || (*tkn == ':'))    // skip spaces and colon
                    tkn++;
                rtsp_th->rtsp_queue->pathname = tkn;
                rtsp_th->type = CONTAINER;
            }
        }

        // media setup
        if (set_rtsp_media(rtsp_th))
            return 1;

        break;
    case DESCRIPTION_MH_FORMAT:
        /* not yet implemented */
        // break;
    default:
        nms_printf(NMSML_ERR, "Unknown decription format.\n");
        return 1;
        break;
    }

    return 0;
}
