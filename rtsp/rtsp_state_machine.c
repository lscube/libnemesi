/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#include <nemesi/rtspinternals.h>
#include <nemesi/methods.h>

#include <nemesi/version.h>
#include <stdarg.h>
#include <fcntl.h>

int (*state_machine[STATES_NUM]) (rtsp_thread *, short);

void rtsp_clean(void *rtsp_thrd)
{
    // rtsp_ctrl *rtsp_ctl = (rtsp_ctrl *)rtsp_control;
    rtsp_thread *rtsp_th = (rtsp_thread *) rtsp_thrd;
    int n;
#ifdef USE_UIPRINTF
    char optstr[256];
#endif                // USE_UIPRINTF

#if 1                
    // We must read last teardown reply from server
    nms_printf(NMSML_DBG1, "Waiting for last Teardown response\n");
    if ((*(rtsp_th->waiting_for)) && nmst_is_active(&rtsp_th->transport)) {
        if ((n = rtsp_recv(rtsp_th)) < 0)
            nms_printf(NMSML_WARN,
                   "No teardown response received\n");
        else if (n > 0) {
            if (full_msg_rcvd(rtsp_th))
                /*if ( */
                handle_rtsp_pkt(rtsp_th);    /*) */
            /*nms_printf(NMSML_ERR, "\nError!\n"); */
        } else
            nms_printf(NMSML_ERR, "Server died prematurely!\n");
    }
#endif
    rtsp_reinit(rtsp_th);
    nms_printf(NMSML_DBG1, "RTSP Thread R.I.P.\n");
#ifdef USE_UIPRINTF
    fprintf(stderr, "\r");    /* TODO Da ottimizzare */
    while ((n = read(UIINPUT_FILENO, optstr, 1)) > 0)
        write(STDERR_FILENO, optstr, n);
#endif                // USE_UIPRINTF
}


static void clean_rtsp_th(rtsp_thread *rtsp_th)
{    
    nms_rtsp_interleaved *p;

    free(rtsp_th->server_port);
    free(rtsp_th->urlname);
    free((rtsp_th->in_buffer).data);

    nmst_close(&rtsp_th->transport);
    nmst_init(&rtsp_th->transport);
    rtsp_th->status = INIT;
    memset(rtsp_th->waiting_for, '\0', strlen(rtsp_th->waiting_for));
    rtsp_th->urlname = NULL;
    rtsp_th->server_port = NULL;
    (rtsp_th->in_buffer).size = 0;
    (rtsp_th->in_buffer).data = NULL;
    rtsp_th->rtsp_queue = NULL;

    // Remove busy state if pending
    rtsp_unbusy(rtsp_th);
    rtsp_th->response_id = -1;

    // reset first RP port
    if (rtsp_th->hints
        || ((rtsp_th->hints->first_rtp_port > RTSP_MIN_RTP_PORT)
        && (rtsp_th->hints->first_rtp_port < 65535))) {
        rtsp_th->force_rtp_port = rtsp_th->hints->first_rtp_port;
        if (rtsp_th->force_rtp_port % 2)
            rtsp_th->force_rtp_port++;
    } else
        rtsp_th->force_rtp_port = 0;

    //destroy interleaved structure
    p = rtsp_th->interleaved;
    while (p) {
        nms_rtsp_interleaved *pp = p->next;
        if (p->rtp_fd > 0)
            close(p->rtp_fd);
        if (p->rtcp_fd > 0)
            close(p->rtcp_fd);
        free(p);
        p = pp;
    }
    rtsp_th->interleaved = NULL;
    rtsp_th->next_ilvd_ch = 0;
}

int rtsp_reinit(rtsp_thread * rtsp_th)
{
    rtsp_medium *med, *pmed;
    rtsp_session *sess, *psess;
    void *ret;

    if (!(sess = psess = rtsp_th->rtsp_queue)) {
        clean_rtsp_th(rtsp_th);
        return 0;
    }
#if 1                // TODO: fix last teardown response wait
    // check for active rtp/rtcp session
    if (sess->media_queue && sess->media_queue->rtp_sess) {
        if (rtsp_th->rtp_th->rtcp_tid > 0) {
            nms_printf(NMSML_DBG1,
                   "Sending cancel signal to RTCP Thread (ID: %lu)\n",
                   rtsp_th->rtp_th->rtcp_tid);
            if ( pthread_cancel(rtsp_th->rtp_th->rtcp_tid) )
                nms_printf(NMSML_DBG2,
                       "Error while sending cancelation to RTCP Thread.\n");
            else {
                if ( pthread_join(rtsp_th->rtp_th->rtcp_tid,
                         (void **) &ret) )
                    nms_printf(NMSML_ERR, "Could not join RTCP Thread!\n");
                else if (ret != PTHREAD_CANCELED)
                    nms_printf(NMSML_DBG2,
                       "Warning! RTCP Thread joined, but  not canceled!\n");
            }
            rtsp_th->rtp_th->rtcp_tid = 0;
        }
        if (rtsp_th->rtp_th->rtp_tid > 0) {
            nms_printf(NMSML_DBG1,
                   "Sending cancel signal to RTP Thread (ID: %lu)\n",
                   rtsp_th->rtp_th->rtp_tid);
            if (pthread_cancel(rtsp_th->rtp_th->rtp_tid) != 0)
                nms_printf(NMSML_DBG2,
                       "Error while sending cancelation to RTP Thread.\n");
            else {
                if ( pthread_join(rtsp_th->rtp_th->rtp_tid,
                         (void **) &ret) )
                    nms_printf(NMSML_ERR, "Could not join RTP Thread!\n");
                else if (ret != PTHREAD_CANCELED)
                    nms_printf(NMSML_DBG2,
                       "Warning! RTP Thread joined, but not canceled.\n");
            }
            rtsp_th->rtp_th->rtp_tid = 0;
        }
    }
#endif
    // the destruction of sdp informations must be done only once, because
    // in all other sessions the pointer is the same and the allocated
    // struct is one
    sdp_session_destroy(sess->info);    //!< free sdp description info
    free(sess->body);
    free(sess->content_base);
    while (sess) {
        // MUST be done only once
        // sdp_session_destroy(sess->info); //!< free sdp description info
        for (med = sess->media_queue; med;
             pmed = med, med = med->next, free(pmed));
        /* like these
           med=pmed=sess->media_queue;
           while(med != NULL){
           pmed=med;
           med=med->next;
           free(pmed);
           }
         */
        psess = sess;
        sess = sess->next;
        free(psess);
    }

    clean_rtsp_th(rtsp_th);

    return 0;
}


/**
 * RTSP packets handling main loop
 * Each rtsp controller forks a thread running this loop to handle incoming answers
 * If there are packets pending on the incoming transports it will call handle_rtsp_pkt
 * @param rtsp_thrd The rtsp thread/controller for which to run the loop
 */
void *rtsp(void *rtsp_thrd)
{
    rtsp_thread *rtsp_th = (rtsp_thread *) rtsp_thrd;
    fd_set readset;
    int n, max_fd;
    nms_rtsp_interleaved *p;
    char buffer[RTSP_BUFFERSIZE];
#ifdef HAVE_LIBSCTP
    struct sctp_sndrcvinfo sinfo;
#endif

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    pthread_cleanup_push(rtsp_clean, rtsp_thrd);

    while (1) {
        FD_ZERO(&readset);
        max_fd = 0;

        if (nmst_is_active(&rtsp_th->transport)) {
            FD_SET(rtsp_th->transport.sock.fd, &readset);
            max_fd = max(rtsp_th->transport.sock.fd, max_fd);
        }

        for (p = rtsp_th->interleaved; p; p = p->next) {
            if (p->rtcp_fd >= 0) {
                FD_SET(p->rtcp_fd, &readset);
                max_fd = max(p->rtcp_fd, max_fd);
            }
        }

       if ( (max_fd) && (select(max_fd + 1, &readset, NULL, NULL, NULL) < 0) ) {
                nms_printf(NMSML_FATAL, "(%s) %s\n", PROG_NAME, strerror(errno));
                pthread_exit(NULL);
       }

        if (nmst_is_active(&rtsp_th->transport))
            if (FD_ISSET(rtsp_th->transport.sock.fd, &readset)) {
                if ((n = rtsp_recv(rtsp_th)) < 0)
                    pthread_exit(NULL);
                else if (n == 0) {
                    nms_printf(NMSML_ERR,
                           "Server died prematurely!\n");
                    rtsp_reinit(rtsp_th);
                    nms_printf(NMSML_NORM,
                           "Session closed.\n");
                } else {
                    while (rtsp_th->in_buffer.size > 0 && full_msg_rcvd(rtsp_th))
                        if (handle_rtsp_pkt(rtsp_th)) {
                            nms_printf(NMSML_ERR, "\nError!\n");
                            rtsp_reinit(rtsp_th);
                        }
                } 
            }

        for (p = rtsp_th->interleaved; p; p = p->next) {
            if (p->rtcp_fd >= 0 && FD_ISSET(p->rtcp_fd, &readset)) {
                switch (rtsp_th->transport.sock.socktype) {
                case TCP:
                    n = recv(p->rtcp_fd, buffer+4, RTSP_BUFFERSIZE-4, 0);
                    buffer[0]='$';
                    buffer[1]= p->proto.tcp.rtcp_ch;
                    *((uint16 *) &buffer[2]) = htons((uint16) n);
                    nmst_write(&rtsp_th->transport, buffer, n+4, NULL);
                    nms_printf(NMSML_DBG2,
                           "Sent RTCP packet on channel %u.\n",
                           buffer[1]);
                    break;
#ifdef HAVE_LIBSCTP
                case SCTP:
                    n = recv(p->rtcp_fd, buffer, RTSP_BUFFERSIZE, 0);
                    memset(&sinfo, 0, sizeof(sinfo));
                    sinfo.sinfo_stream = p->proto.sctp.rtcp_st;
                    sinfo.sinfo_flags = SCTP_UNORDERED;
                    nmst_write(&rtsp_th->transport, buffer, n, &sinfo);
                    nms_printf(NMSML_DBG2,
                           "Sent RTCP packet on stream %u.\n",
                           sinfo.sinfo_stream);
                    break;
#endif
                default:
                    recv(p->rtcp_fd, buffer, RTSP_BUFFERSIZE, 0);
                    nms_printf(NMSML_DBG2,
                       "Unable to send RTCP interleaved packet.\n");
                    break;
                }
            }
        }
    }

    pthread_cleanup_pop(1);
/*    return NULL; */
}

/**
 * Handles RTSP events when the controller is in INIT state
 * @param rtsp_th The controller for which to handle the events
 * @param event The event generated by handle_rtsp_pkt
 * @return 0 if everything is ok, 1 on error
 */
int init_state(rtsp_thread * rtsp_th, short event)
{
    switch (event) {
    case RTSP_GET_RESPONSE:
        if (handle_get_response(rtsp_th))    //{
            // close(rtsp_th->fd);
            // rtsp_th->fd=-1;
            return 1;
        // }
        // get_curr_sess(NULL, NULL, NULL);
        get_curr_sess(GCS_UNINIT);
        /*
           if (get_curr_sess(rtsp_th, NULL, NULL))
           return 1;
         */
        get_curr_sess(GCS_INIT, rtsp_th);
        if (send_setup_request(rtsp_th))    // {
            // rtsp_reinit(rtsp_th);        
            return 1;
        // }
        break;

    case RTSP_SETUP_RESPONSE:
        if (handle_setup_response(rtsp_th))    // {
            // rtsp_reinit(rtsp_th);        
            return 1;
        // }
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        if (!get_curr_sess(GCS_NXT_MED)) {
            /* Nessun altra SETUP da inviare */
            rtsp_th->rtp_th->rtp_sess_head =
                rtsp_th->rtsp_queue->media_queue->rtp_sess;
            /* Esecuzione del Thread RTP: uno per ogni sessione RTSP */
            if (rtp_thread_create(rtsp_th->rtp_th))
                return nms_printf(NMSML_FATAL,
                          "Cannot create RTP Thread!\n");

            /* Esecuzione del Thread RTCP: uno per ogni sessione RTSP */
            if (rtcp_thread_create(rtsp_th->rtp_th))
                return nms_printf(NMSML_FATAL,
                          "Cannot create RTCP Thread!\n");

            rtsp_th->status = READY;
            rtsp_unbusy(rtsp_th);
            // rtsp_th->busy = 0;
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
        }
        if (send_setup_request(rtsp_th))
            return 1;
        break;
    default:
        nms_printf(NMSML_ERR,
               "Could not handle method in INIT state\n");
        return 1;
        break;
    }
    return 0;
}

/**
 * Handles RTSP events when the controller is in READY state
 * @param rtsp_th The controller for which to handle the events
 * @param event The event generated by handle_rtsp_pkt
 * @return 0 if everything is ok, 1 on error
 */
int ready_state(rtsp_thread * rtsp_th, short event)
{
    int last_response_id;

    switch (event) {
    case RTSP_PLAY_RESPONSE:
        if (handle_play_response(rtsp_th))
            return 1;
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        if (!get_curr_sess(GCS_NXT_SESS)) {
            /* Nessun altra PLAY da inviare */
            rtsp_th->status = PLAYING;
            rtsp_unbusy(rtsp_th);
            // rtsp_th->busy = 0;
            nms_printf(NMSML_NORM, "----- Playing... -----\n");
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
        }
        if (send_play_request(rtsp_th, ""))
            return 1;
        break;
    case RTSP_CLOSE_RESPONSE:
        if (handle_teardown_response(rtsp_th))
            return 1;
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        //if (!get_curr_sess(GCS_NXT_MED)) {
            /* Nessun altra TEARDOWN da inviare */
            rtsp_th->status = INIT;
            last_response_id = rtsp_th->response_id;
            rtsp_reinit(rtsp_th);
            rtsp_th->response_id = last_response_id;
            rtsp_unbusy(rtsp_th);
            nms_printf(NMSML_NORM,
                   "----- All Connections closed -----\n");
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
      /*  }
        if (send_teardown_request(rtsp_th))
            return 1;
        break;*/
    default:
        nms_printf(NMSML_ERR,
               "Could not handle method in READY state\n");
        return 1;
        break;

    }
    return 0;
}

/**
 * Handles RTSP events when the controller is in PLAYING state
 * @param rtsp_th The controller for which to handle the events
 * @param event The event generated by handle_rtsp_pkt
 * @return 0 if everything is ok, 1 on error
 */
int playing_state(rtsp_thread * rtsp_th, short event)
{
    int last_response_id;

    switch (event) {
    case RTSP_PAUSE_RESPONSE:
        if (handle_pause_response(rtsp_th))
            return 1;
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        if (!get_curr_sess(GCS_NXT_SESS)) {
            /* Nessun altra PLAY da inviare */
            rtsp_th->status = READY;
            rtsp_unbusy(rtsp_th);
            // rtsp_th->busy = 0;
            nms_printf(NMSML_NORM, "----- Play paused -----\n");
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
        }
        if (send_pause_request(rtsp_th, ""))
            return 1;
        break;
    case RTSP_CLOSE_RESPONSE:
        if (handle_teardown_response(rtsp_th))
            return 1;
        // if (get_curr_sess(rtsp_th, NULL, NULL)) {
        //if (!get_curr_sess(GCS_NXT_MED)) {
            /* Nessun altra TEARDOWN da inviare */
            rtsp_th->status = INIT;
            last_response_id = rtsp_th->response_id;
            rtsp_reinit(rtsp_th);
            rtsp_th->response_id = last_response_id;
            rtsp_unbusy(rtsp_th);
            // rtsp_th->busy = 0;
            nms_printf(NMSML_NORM,
                   "----- All Connections closed -----\n");
            /* Inizializza a NULL le variabili statiche interne */
            // get_curr_sess(NULL, NULL, NULL);
            get_curr_sess(GCS_UNINIT);
            break;
        /*}
        if (send_teardown_request(rtsp_th))
            return 1;
        break;*/
    default:
        nms_printf(NMSML_ERR,
               "Could not handle method in PLAYING state\n");
        return 1;
        break;

    }
    return 0;
}

/**
 * Handles RTSP events when the controller is in RECORDING state.
 * Every event in this state will do nothing, because recording is not implemented
 * @param rtsp_th The controller for which to handle the events
 * @param event The event generated by handle_rtsp_pkt
 * @return Always 0, because recording is not implemented
 */
int recording_state(rtsp_thread * rtsp_th, short event)
{
    switch (event) {
    default:
        nms_printf(NMSML_WARN,
               "Event %d in RTSP state %d (RECORDING) not yet implemented!\n",
               event, rtsp_th->status);
        break;
    }
    return 0;
}


