/* * 
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#include <nemesi/rtspinternals.h>
#include <nemesi/utils.h>
#include <stdarg.h>

void *get_curr_sess(int cmd, ...)
{
    va_list ap;
    rtsp_thread *rtsp_th;
    static rtsp_session *static_sess = NULL;
    static rtsp_medium *static_med = NULL;

    switch (cmd) {
    case GCS_INIT:
        va_start(ap, cmd);
        rtsp_th = va_arg(ap, rtsp_thread *);
        static_sess = rtsp_th->rtsp_queue;
        static_med = static_sess->media_queue;
        va_end(ap);
        break;
    case GCS_NXT_SESS:
        if (static_sess)
            static_sess = static_sess->next;
        if (static_sess)
            static_med = static_sess->media_queue;
        else
            static_med = NULL;
    case GCS_CUR_SESS:
        return static_sess;
        break;
    case GCS_NXT_MED:
        /* sessione corrente, prossimo media */
        if (static_med)
            static_med = static_med->next;
        /* prossima sessione, primo media */
        if ((!static_med) && static_sess) {
            static_sess = static_sess->next;
            if (static_sess)
                static_med = static_sess->media_queue;
        }
    case GCS_CUR_MED:
        return static_med;
        break;
    case GCS_UNINIT:
        static_sess = NULL;
        static_med = NULL;
        break;
    default:
        break;
    }

    return NULL;
}

rtsp_session *rtsp_sess_dup(rtsp_session * curr_rtsp_s)
{
    rtsp_session *new_rtsp_s;

    if ((new_rtsp_s =
         (rtsp_session *) malloc(sizeof(rtsp_session))) == NULL) {
        nms_printf(NMSML_FATAL, "Cannot allocate memory.\n");
        return NULL;
    }

    memcpy(new_rtsp_s, curr_rtsp_s, sizeof(rtsp_session));

    new_rtsp_s->Session_ID = 0;
    new_rtsp_s->next = NULL;

    return new_rtsp_s;
}

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
            if (!strncasecmp(sdp_a->name, "control", 7)) {
                tkn = sdp_a->value;    // 7 == strlen("control")
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


