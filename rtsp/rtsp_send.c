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
#include <nemesi/version.h>
#include <nemesi/methods.h>
#include <nemesi/transport.h>
#include <nemesi/cc.h>

int send_get_request(rtsp_thread * rtsp_th)
{
    char *buf = malloc(strlen(rtsp_th->urlname)+256); //XXX use vla

        if (buf == NULL) return 1;

    /* save the url string for future use in setup request. */
    sprintf(buf, "%s %s %s" RTSP_EL "CSeq: %d" RTSP_EL, GET_TKN,
        rtsp_th->urlname, RTSP_VER, 1);
    strcat(buf, "Accept: application/sdp;" RTSP_EL);    /* application/x-rtsp-mh"RTSP_EL); */
    sprintf(buf + strlen(buf),
        "User-Agent: %s - %s -- Release %s (%s)" RTSP_EL, PROG_NAME,
        PROG_DESCR, VERSION, VERSION_NAME);
    strcat(buf, RTSP_EL);

    sprintf(rtsp_th->waiting_for, "%d", RTSP_GET_RESPONSE);

    nms_printf(NMSML_DBG2, "Sending DESCRIBE request: %s\n", buf);

    if (!nmst_write(&rtsp_th->transport, buf, strlen(buf), NULL)) {
        nms_printf(NMSML_ERR, "Cannot send DESCRIBE request...\n");
        rtsp_th->waiting_for[0] = '\0';
                free(buf);
        return 1;
    }

        free(buf);
    return 0;
}


int send_pause_request(rtsp_thread * rtsp_th, char *range)
{
    char b[256+strlen(rtsp_th->urlname)];
    rtsp_session *rtsp_sess;

    // get_curr_sess(NULL, &rtsp_sess, NULL);
    rtsp_sess = get_curr_sess(GCS_CUR_SESS);

    if (rtsp_sess->content_base != NULL)
        if (*(rtsp_sess->pathname) != 0)
            sprintf(b, "%s %s/%s %s" RTSP_EL "CSeq: %d" RTSP_EL,
                PAUSE_TKN, rtsp_sess->content_base,
                rtsp_sess->pathname, RTSP_VER,
                ++(rtsp_sess->CSeq));
        else
            sprintf(b, "%s %s %s" RTSP_EL "CSeq: %d" RTSP_EL,
                PAUSE_TKN, rtsp_sess->content_base, RTSP_VER,
                ++(rtsp_sess->CSeq));
    else
        sprintf(b, "%s %s %s" RTSP_EL "CSeq: %d" RTSP_EL, PAUSE_TKN,
            rtsp_sess->pathname, RTSP_VER, ++(rtsp_sess->CSeq));

    if (rtsp_sess->Session_ID != 0)    /* must add session ID? */
        sprintf(b + strlen(b), "Session: %"SCNu64 RTSP_EL,
            rtsp_sess->Session_ID);
    if (range && *range)
        sprintf(b + strlen(b), "Range: %s" RTSP_EL, range);
    else
        sprintf(b + strlen(b), "Range: time=0-" RTSP_EL);

    strcat(b, RTSP_EL);

    sprintf(rtsp_th->waiting_for, "%d:%"SCNu64".%d", RTSP_PAUSE_RESPONSE,
        rtsp_sess->Session_ID, rtsp_sess->CSeq);

    if (!nmst_write(&rtsp_th->transport, b, strlen(b), NULL)) {
        nms_printf(NMSML_ERR, "Cannot send PAUSE request...\n");
        rtsp_th->waiting_for[0] = '\0';
        return 1;
    }

    return 0;
}

int send_play_request(rtsp_thread * rtsp_th, char *range)
{
    char b[256+strlen(rtsp_th->urlname)];
    rtsp_session *rtsp_sess;
    rtsp_medium *rtsp_med;
    cc_perm_mask cc_mask, cc_conflict;

    // get_curr_sess(NULL, &rtsp_sess, NULL);
    if (!(rtsp_sess = get_curr_sess(GCS_CUR_SESS)))
        return 1;

    // CC License check

    rtsp_med = rtsp_sess->media_queue;
    memset(&cc_conflict, 0, sizeof(cc_conflict));
    while (rtsp_med) {
        memcpy(&cc_mask, &rtsp_th->accepted_CC, sizeof(cc_perm_mask));
//              pref2ccmask(&cc_mask);
        if (cc_perm_chk(rtsp_med->medium_info->cc, &cc_mask))
            *((CC_BITMASK_T *) & cc_conflict) |=
                *((CC_BITMASK_T *) & cc_mask);
        rtsp_med = rtsp_med->next;
    }
    if (*((CC_BITMASK_T *) & cc_conflict)) {
        nms_printf(NMSML_ERR,
               "You didn't accept some requested conditions of license:\n");
        cc_printmask(cc_conflict);
        return 1;
    }
    // end of CC part

    if (rtsp_sess->content_base != NULL)
        if (*(rtsp_sess->pathname) != 0)
            sprintf(b, "%s %s/%s %s" RTSP_EL "CSeq: %d" RTSP_EL,
                PLAY_TKN, rtsp_sess->content_base,
                rtsp_sess->pathname, RTSP_VER,
                ++(rtsp_sess->CSeq));
        else
            sprintf(b, "%s %s %s" RTSP_EL "CSeq: %d" RTSP_EL,
                PLAY_TKN, rtsp_sess->content_base, RTSP_VER,
                ++(rtsp_sess->CSeq));
    else
        sprintf(b, "%s %s %s" RTSP_EL "CSeq: %d" RTSP_EL, PLAY_TKN,
            rtsp_sess->pathname, RTSP_VER, ++(rtsp_sess->CSeq));

    if (rtsp_sess->Session_ID != 0)    /*must add session ID? */
        sprintf(b + strlen(b), "Session: %"SCNu64 RTSP_EL,
            rtsp_sess->Session_ID);

    if (range && *range)
        sprintf(b + strlen(b), "Range: %s" RTSP_EL, range);
    else
        sprintf(b + strlen(b), "Range: time=0-" RTSP_EL);

    strcat(b, RTSP_EL);

    sprintf(rtsp_th->waiting_for, "%d:%"SCNu64".%d", RTSP_PLAY_RESPONSE,
        rtsp_sess->Session_ID, rtsp_sess->CSeq);

    nms_printf(NMSML_DBG2, "Sending PLAY request: %s\n", b);

    if (!nmst_write(&rtsp_th->transport, b, strlen(b), NULL)) {
        nms_printf(NMSML_ERR, "Cannot send PLAY request...\n");
        rtsp_th->waiting_for[0] = '\0';
        return 1;
    }

    return 0;
}

static int server_create(char *host, char *port, int *sock)
{
    int n;
    struct addrinfo *res, *ressave;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_flags = AI_PASSIVE;
#ifdef IPV6
    hints.ai_family = AF_UNSPEC;
#else
    hints.ai_family = AF_INET;
#endif
    hints.ai_socktype = SOCK_DGRAM;

    if ((n = gethostinfo(&res, host, port, &hints)) != 0)
        return nms_printf(NMSML_ERR, "(%s) %s\n", PROG_NAME,
                  gai_strerror(n));

    ressave = res;

    do {
        if ((*sock =
             socket(res->ai_family, res->ai_socktype,
                res->ai_protocol)) < 0)
            continue;

        if (bind(*sock, res->ai_addr, res->ai_addrlen) == 0)
            break;

        if (close(*sock) < 0)
            return nms_printf(NMSML_ERR, "(%s) %s\n", PROG_NAME,
                      strerror(errno));


    } while ((res = res->ai_next) != NULL);

    freeaddrinfo(ressave);

    return res ? 0 : 1;
}

int send_setup_request(rtsp_thread * rtsp_th)
{

    char b[256 + strlen(rtsp_th->urlname)]; //XXX doublecheck
    char *options = NULL;
    rtsp_session *rtsp_sess;
    rtsp_medium *rtsp_med;
    struct sockaddr_storage rtpaddr, rtcpaddr;
    socklen_t rtplen = sizeof(rtpaddr), rtcplen = sizeof(rtcpaddr);
    nms_rtsp_interleaved *p;
    int sock_pair[2];
    unsigned int rnd;

    memset(b, 0, 256 + strlen(rtsp_th->urlname));

    // if ( get_curr_sess(NULL, &rtsp_sess, &rtsp_med))
    if (!(rtsp_sess = get_curr_sess(GCS_CUR_SESS))
        || !(rtsp_med = get_curr_sess(GCS_CUR_MED)))
        return 1;

    if (!rtsp_th->force_rtp_port) {
        // default behaviour: random port number generation.
        rnd = (rand() % ((2 << 15) - 1 - 5001)) + 5001;

        if ((rnd % 2))
            rnd++;
    } else {
        // RTP port number partially specified by user.
        if (rtsp_th->force_rtp_port % 2) {
            rtsp_th->force_rtp_port++;
            nms_printf(NMSML_WARN,
                   "First RTP port specified was odd number => corrected to %u\n",
                   rtsp_th->force_rtp_port);
        }
        rnd = rtsp_th->force_rtp_port;
    }

    rtsp_med->rtp_sess->transport.type = rtsp_th->default_rtp_proto;
    switch (rtsp_med->rtp_sess->transport.type) {
    case UDP:

        sprintf(b, "%d", rnd);
        server_create(NULL, b, &(rtsp_med->rtp_sess->transport.RTP.sock.fd));

        sprintf(b, "%d", rnd + 1);
        server_create(NULL, b, &(rtsp_med->rtp_sess->transport.RTCP.sock.fd));

        /* per sapere il numero di porta assegnato */
        /* assigned ports */
        getsockname(rtsp_med->rtp_sess->transport.RTP.sock.fd,
                (struct sockaddr *) &rtpaddr, &rtplen);
        getsockname(rtsp_med->rtp_sess->transport.RTCP.sock.fd,
                (struct sockaddr *) &rtcpaddr, &rtcplen);

        rtsp_med->rtp_sess->transport.RTP.sock.local_port =
            ntohs(sock_get_port((struct sockaddr *) &rtpaddr));
        rtsp_med->rtp_sess->transport.RTCP.sock.local_port =
            ntohs(sock_get_port((struct sockaddr *) &rtcpaddr));

        if (set_transport_str(rtsp_med->rtp_sess, &options))
            return 1;

        // next rtp port forces
        if (rtsp_th->force_rtp_port) {
            rtsp_th->force_rtp_port += 2;
            nms_printf(NMSML_DBG2, "Next client ports will be %u-%u\n",
                   rtsp_th->force_rtp_port,
                   rtsp_th->force_rtp_port + 1);
        }
        break;
    case TCP:
        rtsp_med->rtp_sess->transport.RTP.u.tcp.ilvd =
            (rtsp_th->next_ilvd_ch)++;

        rtsp_med->rtp_sess->transport.RTCP.u.tcp.ilvd =
            (rtsp_th->next_ilvd_ch)++;

        if (set_transport_str(rtsp_med->rtp_sess, &options))
            return 1;

        if (!(p = calloc(1, sizeof(nms_rtsp_interleaved)))) {
            nms_printf(NMSML_ERR,
                   "Unable to allocate memory for interleaved struct!\n");
            return 1;
        }
        p->proto.tcp.rtp_ch = rtsp_med->rtp_sess->transport.RTP.u.tcp.ilvd;
        p->proto.tcp.rtcp_ch = rtsp_med->rtp_sess->transport.RTCP.u.tcp.ilvd;
        
        if (socketpair(PF_UNIX, SOCK_DGRAM, 0, sock_pair) < 0) {
            nms_printf(NMSML_ERR,
                   "Unable to allocate memory for interleaved struct!\n");
            free(p);
            return 1;
        }
        rtsp_med->rtp_sess->transport.RTP.sock.fd = sock_pair[0];
        p->rtp_fd = sock_pair[1];

        if (socketpair(PF_UNIX, SOCK_DGRAM, 0, sock_pair) < 0) {
            nms_printf(NMSML_ERR,
                   "Unable to allocate memory for interleaved struct!\n");
            close(rtsp_med->rtp_sess->transport.RTP.sock.fd);
            close(p->rtp_fd);
            free(p);
            return 1;
        }
        rtsp_med->rtp_sess->transport.RTCP.sock.fd = sock_pair[0];
        p->rtcp_fd = sock_pair[1];

        nms_printf(NMSML_DBG1, "Interleaved RTP local sockets: %d <-> %d\n",
               rtsp_med->rtp_sess->transport.RTP.sock.fd, p->rtp_fd);

        nms_printf(NMSML_DBG1, "Interleaved RTCP local sockets: %d <-> %d\n",
               rtsp_med->rtp_sess->transport.RTCP.sock.fd, p->rtcp_fd);

        p->next = rtsp_th->interleaved;
        rtsp_th->interleaved = p;

        break;
    case SCTP:
        rtsp_med->rtp_sess->transport.RTP.u.sctp.stream =
            ++(rtsp_th->next_ilvd_ch);

        rtsp_med->rtp_sess->transport.RTCP.u.sctp.stream =
            ++(rtsp_th->next_ilvd_ch);

        if (set_transport_str(rtsp_med->rtp_sess, &options))
            return 1;

        if (!(p = calloc(1, sizeof(nms_rtsp_interleaved)))) {
            nms_printf(NMSML_ERR,
                   "Unable to allocate memory for interleaved struct!\n");
            return 1;
        }
        p->proto.sctp.rtp_st = rtsp_med->rtp_sess->transport.RTP.u.sctp.stream;
        p->proto.sctp.rtcp_st = rtsp_med->rtp_sess->transport.RTCP.u.sctp.stream;

        if (socketpair(PF_UNIX, SOCK_DGRAM, 0, sock_pair) < 0) {
            nms_printf(NMSML_ERR,
                   "Unable to allocate memory for interleaved struct!\n");
            free(p);
            return 1;
        }
        rtsp_med->rtp_sess->transport.RTP.sock.fd = sock_pair[0];
        p->rtp_fd = sock_pair[1];

        if (socketpair(PF_UNIX, SOCK_DGRAM, 0, sock_pair) < 0) {
            nms_printf(NMSML_ERR,
                   "Unable to allocate memory for interleaved struct!\n");
            close(rtsp_med->rtp_sess->transport.RTP.sock.fd);
            close(p->rtp_fd);
            free(p);
            return 1;
        }
        rtsp_med->rtp_sess->transport.RTCP.sock.fd = sock_pair[0];
        p->rtcp_fd = sock_pair[1];

        p->next = rtsp_th->interleaved;
        rtsp_th->interleaved = p;

        break;
    default:
        return 1;
    }

    if (rtsp_sess->content_base != NULL)
        sprintf(b, "%s %s/%s %s" RTSP_EL, SETUP_TKN,
            rtsp_sess->content_base, rtsp_med->filename, RTSP_VER);
    else
        sprintf(b, "%s %s %s" RTSP_EL, SETUP_TKN, rtsp_med->filename,
            RTSP_VER);
    sprintf(b + strlen(b), "CSeq: %d" RTSP_EL, ++(rtsp_sess->CSeq));
    sprintf(b + strlen(b), "Transport: %s" RTSP_EL, options);

    if (rtsp_sess->Session_ID)    //Caso di controllo aggregato: � gi� stato definito un numero per la sessione corrente.
        sprintf(b + strlen(b), "Session: %"SCNu64 RTSP_EL,
        rtsp_sess->Session_ID);

    strcat(b, RTSP_EL);

    sprintf(rtsp_th->waiting_for, "%d.%d", RTSP_SETUP_RESPONSE,
        rtsp_sess->CSeq);

    nms_printf(NMSML_DBG2, "Sending SETUP request: %s\n", b);

    if (!nmst_write(&rtsp_th->transport, b, strlen(b), NULL)) {
        nms_printf(NMSML_ERR, "Cannot send SETUP request...\n");
        free(options);
        rtsp_th->waiting_for[0] = '\0';
        return 1;
    }




    free(options);

    return 0;
}

int send_teardown_request(rtsp_thread * rtsp_th)
{

    char b[256 + strlen(rtsp_th->urlname)];
    rtsp_session *rtsp_sess;
    rtsp_medium *rtsp_med;

    memset(b, 0, 256 + strlen(rtsp_th->urlname));

    // if ( get_curr_sess(NULL, &rtsp_sess, &rtsp_med))
    if (!(rtsp_sess = get_curr_sess(GCS_CUR_SESS))
        || !(rtsp_med = get_curr_sess(GCS_CUR_MED)))
        return 1;

    if (rtsp_sess->content_base != NULL)
        sprintf(b, "%s %s %s" RTSP_EL, CLOSE_TKN,
            rtsp_sess->content_base, RTSP_VER);
    else
        sprintf(b, "%s %s %s" RTSP_EL, CLOSE_TKN, rtsp_med->filename,
            RTSP_VER);

    sprintf(b + strlen(b), "CSeq: %d" RTSP_EL, ++(rtsp_sess->CSeq));
    if (rtsp_sess->Session_ID != 0)    /*must add session ID? */
        sprintf(b + strlen(b), "Session: %"SCNu64 RTSP_EL,
            rtsp_sess->Session_ID);
    strcat(b, RTSP_EL);

    sprintf(rtsp_th->waiting_for, "%d:%"SCNu64".%d", RTSP_CLOSE_RESPONSE,
        rtsp_sess->Session_ID, rtsp_sess->CSeq);

    if (!nmst_write(&rtsp_th->transport, b, strlen(b), NULL)) {
        nms_printf(NMSML_ERR, "Cannot send TEARDOWN request...\n");
        rtsp_th->waiting_for[0] = '\0';
        return 1;
    }


    return 0;
}


