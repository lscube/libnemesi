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

#include "rtspinternals.h"
#include "utils.h"

int body_exists(char *in_buffer)
{
    int body_len = 0;
    char *con_len;

    if ((con_len = strstrcase(in_buffer, "Content-Length")) != NULL) {
        con_len += 14;
        while ((*(con_len) == ' ') || (*(con_len) == ':'))
            con_len++;
        sscanf(con_len, "%d", &body_len);
    }
    return body_len;
}

int check_response(rtsp_thread * rtsp_th)
{
    int wait_res = rtsp_th->wait_for.res;
    int wait_cseq = rtsp_th->wait_for.cseq;
    char *wait_s_id = rtsp_th->wait_for.session_id;
    char Session_ID[256];
    char *str_pos, *content;
    int CSeq;
    int opcode = 0;

    if ((content = strchr(rtsp_th->in_buffer.data, '\n')) == NULL) {
        nms_printf(NMSML_ERR,
               "ERROR: CANNOT find end of line in server response.\n");
        return -1;
    }

    if ((str_pos = strstrcase(content, "CSeq")) == NULL) {
        nms_printf(NMSML_ERR,
               "ERROR: CANNOT find CSeq number in server response.\n");
        return -1;
    }
    str_pos += 5;
    while ((*(str_pos) == ' ') || (*(str_pos) == ':'))
        str_pos++;
    sscanf(str_pos, "%d", &CSeq);
    switch (wait_res) {
    case RTSP_GET_RESPONSE:
        if (CSeq == 1)
            opcode = RTSP_GET_RESPONSE;
        break;
    case RTSP_SETUP_RESPONSE:
        if (CSeq == wait_cseq)
            opcode = RTSP_SETUP_RESPONSE;
        break;
    default:
        if ((str_pos = strstrcase(content, "Session:")) != NULL) {
            str_pos += 8;
            while ((*(str_pos) == ' ') || (*(str_pos) == ':'))
                str_pos++;
            sscanf(str_pos, "%255s", Session_ID);
            if (strcmp(Session_ID, wait_s_id)) {
                nms_printf(NMSML_ERR, "Unexpected SessionID\n");
                break;
            }
        }
        if (CSeq == wait_cseq)
            opcode = wait_res;
        break;
    }
    nms_printf(NMSML_DBG2, "Opcode Set to %d\n", opcode);
    return opcode;
}

/**
 * @brief scan status code of an RTSP reply
 *
 * @param status_line the status line in the reply
 * @return reply status code or -1 on error
 */
int check_status(char *status_line, rtsp_thread * rtsp_th)
{
    char ver[32];
    unsigned short res_state;
    char *reason_phrase;
    char *location = NULL;
    // string tokenizers
    char *tkn, *prev_tkn, *step;

    if (sscanf(status_line, "%31s %hu ", ver, &res_state) < 2) {
        nms_printf(NMSML_ERR,
               "invalid Status-Line in DESCRIBE Response\n");
        return -1;
    }
    reason_phrase = strchr(strchr(status_line, ' ') + 1, ' ') + 1;

    rtsp_th->response_id = res_state;

    if (RTSP_IS_SUCCESS(res_state))
        return res_state;
    else if (RTSP_IS_REDIRECT(res_state)) {
        nms_printf(NMSML_NORM,
               "WARNING: Redirection. reply was: %hu %s\n",
               res_state, reason_phrase);
        switch (res_state) {
        case RTSP_FOUND:
            if ((prev_tkn =
                 strtok_r(rtsp_th->in_buffer.data +
                          strlen(status_line) + 1, "\n", &step)) == NULL) {
                nms_printf(NMSML_ERR,
                       "Could not find \"Location\" so..."
                       " Where I'll redirect you?\n");
                return -1;
            }
            while (((tkn = strtok_r(NULL, "\n", &step)) != NULL)
                   && ((tkn - prev_tkn) > 1)) {
                if (((tkn - prev_tkn) == 2) && (*prev_tkn == '\r'))
                    break;
                if (!strncasecmp(prev_tkn, "Location", 8)) {
                    prev_tkn += 8;
                    while ((*(prev_tkn) == ' ') || (*(prev_tkn) == ':'))
                        prev_tkn++;
                    location = strdup(prev_tkn);
                    // sscanf(prev_tkn,"%d",&location);
                }
                prev_tkn = tkn;
            }
            if (location) {
                nms_printf(NMSML_NORM, "Redirecting to %s\n", location);
                rtsp_open((rtsp_ctrl*)rtsp_th, location);
            } else
                return -nms_printf(NMSML_ERR, "No location string\n");
        }
    } else if (RTSP_IS_CLIENT_ERROR(res_state))
        nms_printf(NMSML_ERR, "Client error. Reply was: %hu %s\n",
               res_state, reason_phrase);
    else if (RTSP_IS_SERVER_ERROR(res_state))
        nms_printf(NMSML_ERR, "Server error. Reply was: %hu %s\n",
               res_state, reason_phrase);
    return -1;
}

int full_msg_rcvd(rtsp_thread * rtsp_th)
{
    struct rtsp_buffer *in_buffer = &rtsp_th->in_buffer;
    char *back_n;       /* pointer to newline */
    char *head_end;     /* pointer to header end */
    size_t body_len;

    // is there an interleaved RTP/RTCP packet?
    if ((rtsp_th->transport.sock.socktype == TCP && rtsp_th->interleaved) &&
        in_buffer->size > 4 && in_buffer->data[0] == '$') {

        if ((body_len = ntohs(*((uint16_t *) &(in_buffer->data[2]))) + 4)
            <= in_buffer->size) {
            in_buffer->first_pkt_size = body_len;
            return 1;
        } else {
            return 0;
        }
    }

    if ((head_end = strchr(in_buffer->data, '\n')) == NULL)
        return 0;
    do {
        back_n = head_end;
        if ((head_end = strchr(head_end + 1, '\n')) == NULL)
            return 0;    /* header is not complete */
        if (((head_end - back_n) == 2) && (*(back_n + 1) == '\r'))
            break;
    } while ((head_end - back_n) > 1);    /* here is the end of header */

    /* seek for first valid char after the empty line */
    while ((*(++head_end) == '\n') || (*head_end == '\r'));

    if ((body_len = body_exists(in_buffer->data)) == 0) {
        in_buffer->first_pkt_size = head_end - in_buffer->data;
        return 1;    /* header received (no payload) */
    }

    if (strlen(head_end) < body_len)
        return 0;    /* body incomplete */

    in_buffer->first_pkt_size = head_end - in_buffer->data + body_len;
    return 1;        /* full message received */
}

void rtsp_unbusy(rtsp_thread * rtsp_th)
{
    pthread_mutex_lock(&(rtsp_th->comm_mutex));

    rtsp_th->busy = RTSP_READY;
    pthread_cond_signal(&(rtsp_th->cond_busy));

    pthread_mutex_unlock(&(rtsp_th->comm_mutex));
}

int remove_pkt(rtsp_thread * rtsp_th)
{
    char *buff = NULL;
    size_t new_size = rtsp_th->in_buffer.size -
                      rtsp_th->in_buffer.first_pkt_size;

    if (new_size) {
        if ((buff = malloc(new_size)) == NULL)
            return nms_printf(NMSML_FATAL,
                              "remove_pkt: Cannot allocate memory!"
                              " (%d bytes)\n", new_size);

        memcpy(buff, rtsp_th->in_buffer.data +
                     rtsp_th->in_buffer.first_pkt_size,
                     rtsp_th->in_buffer.size -
                     rtsp_th->in_buffer.first_pkt_size);
    } else buff = NULL;

    free(rtsp_th->in_buffer.data);
    rtsp_th->in_buffer.data = buff;
    rtsp_th->in_buffer.size -= rtsp_th->in_buffer.first_pkt_size;
    rtsp_th->in_buffer.first_pkt_size = 0;
    return 0;
}

int rtsp_recv(rtsp_thread * rtsp_th)
{
    int n = -1, m = 0;
    char buffer[RTSP_BUFFERSIZE];
    nms_rtsp_interleaved *p;
#ifdef HAVE_LIBSCTP
    struct sctp_sndrcvinfo sinfo;
#endif

    memset(buffer, '\0', RTSP_BUFFERSIZE);

    switch(rtsp_th->transport.sock.socktype) {
    case TCP:
        n = nmst_read(&rtsp_th->transport, buffer, RTSP_BUFFERSIZE, NULL);
        break;
#ifdef HAVE_LIBSCTP
    case SCTP:
        memset(&sinfo, 0, sizeof(sinfo));
        n = nmst_read(&rtsp_th->transport, buffer, RTSP_BUFFERSIZE, &sinfo);
        m = sinfo.sinfo_stream;
        break;
#endif
    default:
        break;
    }
    if (n == 0) {
        return 0;
    }
    if (n < 0) {
        nms_printf(NMSML_ERR, "Could not read from RTSP socket\n");
        return n;
    }
    if (rtsp_th->transport.sock.socktype == TCP ||
        (rtsp_th->transport.sock.socktype == SCTP && m==0)) {
        if ((rtsp_th->in_buffer.size) == 0) {
            if ((rtsp_th->in_buffer.data =
                 (char *) calloc(1, n + 1)) == NULL)
                return nms_printf(NMSML_FATAL,
                  "Cannot alloc memory space for received RTSP data\n");

            memcpy(rtsp_th->in_buffer.data, buffer, n);
        } else {
            if ((rtsp_th->in_buffer.data =
                 (char *) realloc(rtsp_th->in_buffer.data,
                n + rtsp_th->in_buffer.size + 1)) ==
                NULL)
                return nms_printf(NMSML_FATAL,
                  "Cannot alloc memory space for received RTSP data\n");

            memcpy(rtsp_th->in_buffer.data + rtsp_th->in_buffer.size, buffer, n);
    }
    rtsp_th->in_buffer.size += n;
    rtsp_th->in_buffer.data[rtsp_th->in_buffer.size] = '\0';
    } else /* if (rtsp_th->transport.sock.socktype == SCTP && m!=0) */ {
#ifdef HAVE_LIBSCTP
        for (p = rtsp_th->interleaved; p && !((p->proto.sctp.rtp_st == m)
            || (p->proto.sctp.rtcp_st == m)); p = p->next);
        if (p) {
            if (p->proto.sctp.rtp_st == m) {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTP data (%u bytes: channel %d -> sd %d)\n",
                       n, m, p->rtp_fd);
                send(p->rtp_fd, buffer, n, MSG_EOR);
            } else {
                nms_printf(NMSML_DBG2,
                       "Interleaved RTCP data (%u bytes: channel %d -> sd %d)\n",
                       n, m, p->rtcp_fd);
                send(p->rtcp_fd, buffer, n, MSG_EOR);
            }
        }
#endif
    }
    return n;
}

int set_rtsp_media(rtsp_thread * rtsp_th)
{
    rtsp_session *curr_rtsp_s = rtsp_th->rtsp_queue;
    rtsp_medium *curr_rtsp_m = NULL;
    sdp_medium_info *sdp_m;
    sdp_attr *sdp_attr;
    char *tkn, *ch;
    uint8_t pt;

    switch (rtsp_th->descr_fmt) {
    case DESCRIPTION_SDP_FORMAT:
        for (sdp_m = curr_rtsp_s->info->media_info_queue; sdp_m;
             sdp_m = sdp_m->next) {
            if (curr_rtsp_m == NULL) {
                /* first medium */
                if ((curr_rtsp_s->media_queue = curr_rtsp_m =
                     rtsp_med_create(rtsp_th)) == NULL)
                    return 1;
            } else if (rtsp_th->type == CONTAINER) {
                /* media in the same session */
                if ((curr_rtsp_m->next = rtsp_med_create(rtsp_th)) == NULL)
                    return 1;
                curr_rtsp_m->rtp_sess->next =
                    curr_rtsp_m->next->rtp_sess;
                curr_rtsp_m = curr_rtsp_m->next;
            } else if (rtsp_th->type == M_ON_DEMAND) {
                /* one medium for each session */
                if ((curr_rtsp_s->next = rtsp_sess_dup(curr_rtsp_s)) == NULL)
                    return 1;
                curr_rtsp_s = curr_rtsp_s->next;
                if ((curr_rtsp_s->media_queue = rtsp_med_create(rtsp_th)) ==
                    NULL)
                    return 1;
                curr_rtsp_m->rtp_sess->next =
                    curr_rtsp_s->media_queue->rtp_sess;
                curr_rtsp_m = curr_rtsp_s->media_queue;
            }
            curr_rtsp_m->medium_info = sdp_m;

            // setup rtp format list for current media
            for (tkn = sdp_m->fmts;
                 *tkn && !(!(pt = strtoul(tkn, &ch, 10)) && ch == tkn);
                 tkn = ch) {
                switch (sdp_m->media_type) {
                case 'A':
                    if (rtp_announce_pt(curr_rtsp_m->rtp_sess, pt, AU))
                        return 1;
                    break;
                case 'V':
                    if (rtp_announce_pt(curr_rtsp_m->rtp_sess, pt, VI))
                        return 1;
                    break;
                default:
                    // not recognized
                    if (rtp_announce_pt(curr_rtsp_m->rtp_sess, pt, NA))
                        return 1;
                    break;
                }
            }

            for (sdp_attr = sdp_m->attr_list; sdp_attr;
                 sdp_attr = sdp_attr->next) {
                if (!strncasecmp(sdp_attr->name, "control", 7)) {
                    tkn = sdp_attr->value;
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    curr_rtsp_m->filename = tkn;
                } else
                    if (!strncasecmp(sdp_attr->name, "rtpmap", 6))
                {
                    /* We assume the string in the format:
                     * rtpmap:PaloadType EncodingName/ClockRate[/Channels] */
                    tkn = sdp_attr->value;
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if (((pt = strtoul(tkn, &tkn, 10)) >= 96)
                        && (pt <= 127)) {
                        while (*tkn == ' ')
                            tkn++;
                        if (!(ch = strchr(tkn, '/'))) {
                            nms_printf(NMSML_WARN, "Invalid field rtpmap.\n");
                            break;
                        }
                        *ch = '\0';
                        if (rtp_dynpt_reg (curr_rtsp_m->rtp_sess, pt, tkn))
                            return 1;
                        switch (sdp_m->media_type) {
                        case 'A':
                            sscanf(ch + 1, "%u/%c",
                                &curr_rtsp_m->rtp_sess->ptdefs[pt]->rate,
                                &RTP_AUDIO(curr_rtsp_m->rtp_sess->ptdefs[pt])->
                                   channels);
                            break;
                        case 'V':
                            sscanf(ch + 1, "%u",
                                   &curr_rtsp_m->rtp_sess->ptdefs[pt]->rate);
                            break;
                        default:
                            // not recognized
                            break;
                        }
                        *ch = '/';
                        tkn = ++ch;
                    } else {
                        // shawill: should be an error or a warning?
                        nms_printf(NMSML_WARN,
                            "Warning: rtpmap attribute is trying to set a"
                            "non-dynamic payload type: not permitted\n");
                    }
                } else
                    if (!strncasecmp(sdp_attr->name, "fmtp", 4)) {
                    /* We assume the string in the format:
                     * fmtp:PaloadType <format specific parameters> */
                    tkn = sdp_attr->value;    // 4 == strlen("fmtp")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if ((pt = strtoul(tkn, &tkn, 10)) <= 127) {
                        while (*tkn == ' ')
                            tkn++;
                        rtp_pt_attr_add(curr_rtsp_m->rtp_sess->ptdefs, pt, tkn);
                    } else {
                        // shawill: should be an error or a warning?
                        nms_printf(NMSML_WARN,
                            "Warning: fmtp attribute is trying to set an"
                            "out of bounds payload type: not permitted\n");
                    }
                /* dirty keyword from old fenice used for dinamic
                 * payload change - TO BE REMOVED
                 */
                } else if (!strncasecmp(sdp_attr->name, "med", 3)) {
                    sdp_medium_info m_info;
                    /* We assume the string in the format:
                     * med:sdp-like m= field */
                    tkn = sdp_attr->value;    // 3 == strlen("med")
                    // skip spaces and colon (we should not do this!)
                    while ((*tkn == ' ') || (*tkn == ':'))
                        tkn++;
                    if (sdp_parse_m_descr(&m_info, tkn)) {
                        nms_printf(NMSML_ERR,
                               "malformed a=med: from fenice\n");
                        return 1;
                    }
                    // check if everything is correct
                    if (!(pt = strtoul(m_info.fmts, &ch, 10))
                        && ch == m_info.fmts) {
                        nms_printf(NMSML_ERR,
                            "Could not determine pt value in a=med: string"
                            " from fenice\n");
                        return 1;
                    }
                    switch (sdp_m->media_type) {
                    case 'A':
                        if (strncasecmp
                            (tkn, "audio ", 6)) {
                            nms_printf(NMSML_ERR,
                                "a=med; attribute defined a different media"
                                " type than the original\n");
                            return 1;
                        }
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             AU))
                            return 1;
                        break;
                    case 'V':
                        if (strncasecmp
                            (tkn, "video ", 6)) {
                            nms_printf(NMSML_ERR,
                                "a=med; attribute defined a different media"
                                " type than the original\n");
                            return 1;
                        }
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             VI))
                            return 1;
                        break;
                    default:
                        // not recognized
                        if (rtp_announce_pt
                            (curr_rtsp_m->rtp_sess, pt,
                             NA))
                            return 1;
                        break;
                    }
                }
            }
        }
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
