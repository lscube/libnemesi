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

int handle_setup_response(rtsp_thread * rtsp_th)
{
    char *step; /* strtok_r status */
    char *tkn;        /* currently parsed token */
    char *prev_tkn;        /* last parsed token */

    rtsp_session *rtsp_sess;
    rtsp_medium *rtsp_med;

    // if (get_curr_sess(NULL, &rtsp_sess, &rtsp_med))
    if (!(rtsp_sess = get_curr_sess(GCS_CUR_SESS))
        || !(rtsp_med = get_curr_sess(GCS_CUR_MED)))
        return 1;

    nms_printf(NMSML_DBG2, "Got SETUP reply: %s\n", (rtsp_th->in_buffer).data);

    if ((prev_tkn = strtok_r((rtsp_th->in_buffer).data, "\n", &step)) == NULL) {
        nms_printf(NMSML_ERR, "Invalid RTSP-SETUP response\n");
        return 1;
    }

    if (check_status(prev_tkn, rtsp_th) < 0) {
        remove_pkt(rtsp_th);
        return 1;
    }

    while (((tkn = strtok_r(NULL, "\n", &step)) != NULL) && ((tkn - prev_tkn) > 1)) {
        if (((tkn - prev_tkn) == 2) && (*prev_tkn == '\r'))
            break;
        prev_tkn = tkn;

        /* get_transport_str calls strtok itself, that's why we need a strtok_r */
        if (!strncasecmp(prev_tkn, "Transport", 9)) {
            prev_tkn += 9;
            get_transport_str(rtsp_med->rtp_sess, prev_tkn);
        }

        if (!strncasecmp(prev_tkn, "Session", 7)) {
            prev_tkn += 7;
            sscanf(prev_tkn, " : %"SCNu64" ; ",
                   &(rtsp_sess->Session_ID));
        }
    }
    while ((tkn != NULL)
           && ((*tkn == '\r') || (*tkn == '\n') || (*tkn == '\0')))
        tkn = strtok_r(NULL, "\n", &step);    /* cerco l'inizio del body o, eventualmente, del prossimo pkt */
    if (tkn != NULL)
        tkn[strlen(tkn)] = '\n';    /* rimetto a posto il \n modificato dalla strtok */

    remove_pkt(rtsp_th);
    memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));
    return 0;
}

int handle_get_response(rtsp_thread * rtsp_th)
{
    char *tkn;        /* contains an RTSP description line */
    char *prev_tkn;        /* addresses first the previous token in order to check the end of RTSP header 
                   and then the all the components of command line */
    int content_length;
    char *content_base = NULL;

    if ((prev_tkn = strtok((rtsp_th->in_buffer).data, "\n")) == NULL) {
        nms_printf(NMSML_ERR, "Invalid RTSP-DESCRIBE response\n");
        return 1;
    }
    if (check_status(prev_tkn, rtsp_th) < 0) {
        remove_pkt(rtsp_th);
        return 1;
    }
    /* state success: header parsing */
    while (((tkn = strtok(NULL, "\n")) != NULL) && ((tkn - prev_tkn) > 1)) {
        if (((tkn - prev_tkn) == 2) && (*prev_tkn == '\r'))
            break;
        prev_tkn = tkn;
        if (!strncasecmp(prev_tkn, "Content-Length", 14)) {
            prev_tkn += 14;
            while ((*(prev_tkn) == ' ') || (*(prev_tkn) == ':'))
                prev_tkn++;
            sscanf(prev_tkn, "%d", &content_length);
        } else if (!strncasecmp(prev_tkn, "Content-Type", 12)) {
            prev_tkn += 12;
            while ((*(prev_tkn) == ' ') || (*(prev_tkn) == ':'))
                prev_tkn++;
            if (!strncasecmp(prev_tkn, "application/sdp", 15))
                rtsp_th->descr_fmt = DESCRIPTION_SDP_FORMAT;
            // description_format=DESCRIPTION_SDP_FORMAT;
            /* 
               else if ( !strncasecmp(prev_tkn, "application/x-rtsp-mh", 21) )
               description_format=DESCRIPTION_MH_FORMAT;
             */
            else {
                nms_printf(NMSML_ERR,
                       "Content-Type %s not recognized\n",
                       prev_tkn);
            }
        } else if (!strncasecmp(prev_tkn, "Content-Base", 12)) {
            prev_tkn += 12;
            while ((*(prev_tkn) == ' ') || (*(prev_tkn) == ':'))
                prev_tkn++;
            content_base = prev_tkn;
            if (content_base[strlen(content_base) - 1] == '\r')
                content_base[strlen(content_base) - 1] = '\0';
            if (content_base[strlen(content_base) - 1] == '/')
                content_base[strlen(content_base) - 1] = '\0';
        }
    }
    while ((tkn != NULL)
           && ((*tkn == '\r') || (*tkn == '\n') || (*tkn == '\0')))
        tkn = strtok(NULL, "\n");    /* cerco l'inizio del body o, eventualmente del prossimo pkt */
    if (tkn != NULL)
        tkn[strlen(tkn)] = '\n';    /* rimetto a posto il \n modificato dalla strtok */
    // if ( set_rtsp_sessions(rtsp_th, content_length, content_base, tkn, description_format) ) {
    if (set_rtsp_sessions(rtsp_th, content_length, content_base, tkn))
        return 1;
    remove_pkt(rtsp_th);
    memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));
    return 0;
}

int handle_teardown_response(rtsp_thread * rtsp_th)
{
    char *prev_tkn;

    if ((prev_tkn = strtok((rtsp_th->in_buffer).data, "\n")) == NULL) {
        nms_printf(NMSML_ERR, "Invalid RTSP-TEARDOWN response\n");
        return 1;
    }
    if (check_status(prev_tkn, rtsp_th) < 0) {
        remove_pkt(rtsp_th);
        return 1;
    }

    remove_pkt(rtsp_th);
    memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));

    return 0;
}


int handle_pause_response(rtsp_thread * rtsp_th)
{
    char *prev_tkn;

    if ((prev_tkn = strtok((rtsp_th->in_buffer).data, "\n")) == NULL) {
        nms_printf(NMSML_ERR, "Invalid RTSP-PAUSE response\n");
        return 1;
    }
    if (check_status(prev_tkn, rtsp_th) < 0) {
        remove_pkt(rtsp_th);
        return 1;
    }

    remove_pkt(rtsp_th);
    memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));
    return 0;
}

int handle_play_response(rtsp_thread * rtsp_th)
{
    char *prev_tkn;

    if ((prev_tkn = strtok((rtsp_th->in_buffer).data, "\n")) == NULL) {
        nms_printf(NMSML_ERR, "Invalid RTSP-PLAY response\n");
        return 1;
    }
    if (check_status(prev_tkn, rtsp_th) < 0) {
        remove_pkt(rtsp_th);
        return 1;
    }

    remove_pkt(rtsp_th);
    memset(rtsp_th->waiting_for, 0, strlen(rtsp_th->waiting_for));
    return 0;
}


/**
 * Handles any incoming rtsp packet, it is called by the main loop.
 * It then calls the correct handling function depending on the current state of the thread
 * @param rtsp_th The thread for which to handle the pending packets
 * @return 0 or 1 on invalid or unexpected packet
 */
int handle_rtsp_pkt(rtsp_thread * rtsp_th)
{
    char ver[32];
    int opcode;

    if ((rtsp_th->transport.sock.socktype == TCP && rtsp_th->interleaved) && (rtsp_th->in_buffer).data[0] == '$') {
        nms_rtsp_interleaved *p;
        const uint8_t m = ((rtsp_th->in_buffer).data[1]);
#define DATA_PTR (&((rtsp_th->in_buffer).data[4]))
#define DATA_SIZE ((rtsp_th->in_buffer).first_pkt_size - 4)

        for (p = rtsp_th->interleaved; p && !((p->proto.tcp.rtp_ch == m)
            || (p->proto.tcp.rtcp_ch == m)); p = p->next);
        if (p) {
            if (p->proto.tcp.rtcp_ch == m) {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTCP data (%u bytes: channel %d -> sd %d)\n",
                       DATA_SIZE, m, p->rtcp_fd);
                send(p->rtcp_fd, DATA_PTR, DATA_SIZE, 0);
            } else {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTP data (%u bytes: channel %d -> sd %d)\n",
                       DATA_SIZE, m, p->rtp_fd);
                send(p->rtp_fd, DATA_PTR, DATA_SIZE, 0);
            }
        }

        remove_pkt(rtsp_th);
        return 0; /* received rtp interleaved data handled correctly */
    }

    if (sscanf((rtsp_th->in_buffer).data, "%s ", ver) < 1) {
        nms_printf(NMSML_ERR,"Invalid RTSP message received\n");
        return 1;
    }
    if (strncmp(ver, "RTSP", 4) && strncmp(ver, "rtsp", 4)) {
        nms_printf(NMSML_ERR,"Invalid RTSP message received\n");
        return 1;
    }
    if ((opcode = check_response(rtsp_th)) <= 0) {
        nms_printf(NMSML_ERR,"unexpected RTSP packet arrived\n");
        return 1;
    }

    return state_machine[rtsp_th->status] (rtsp_th, opcode);
}
