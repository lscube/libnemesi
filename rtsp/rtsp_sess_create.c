/* * 
 *  $Id:rtsp_sess_create.c 267 2006-01-12 17:19:45Z shawill $
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

rtsp_session *rtsp_sess_create(char *urlname, char *content_base)
{
    rtsp_session *rtsp_s;

    if ((rtsp_s = (rtsp_session *) malloc(sizeof(rtsp_session))) == NULL) {
        nms_printf(NMSML_FATAL,
               "rtsp_sess_create: Cannot allocate memory.\n");
        return NULL;
    }
    if (content_base == NULL) {
        rtsp_s->content_base = NULL;
        rtsp_s->pathname = urlname;
    } else {
        /* shawill: using strdup insted
           if ((rtsp_s->pathname=rtsp_s->content_base=(char *)malloc(strlen(content_base)+1))==NULL) {
           nms_printf(NMSML_FATAL, "Cannot allocate memory!\n");
           return NULL;
           }
           strcpy(rtsp_s->content_base,content_base);
         */
        if (!
            (rtsp_s->pathname = rtsp_s->content_base =
             strdup(content_base)))
            return NULL;
        rtsp_s->pathname += strlen(content_base);
    }
    rtsp_s->Session_ID = 0;
    rtsp_s->CSeq = 1;
    rtsp_s->media_queue = NULL;
    rtsp_s->next = NULL;

    rtsp_s->info = NULL;

    return rtsp_s;
}
