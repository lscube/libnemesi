/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2007 by team@streaming.polito.it
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

#include <nemesi/rtspinternals.h>
#include <nemesi/utils.h>
#include <nemesi/methods.h>
#include <fcntl.h>

/** @file rtsp.c
 * This file contains the interface functions to the rtsp packets and requests handling of the library
 */

#define RET_ERR(ret_level, ...)    do { \
                    nms_printf(ret_level, __VA_ARGS__ ); \
                    free(rtsp_th->comm); \
                    free(rtsp_th); \
                    return NULL; \
                } while (0)

/**
 * Initialized the library and starts a thread that handles both commands to send
 * to the server and responses received from the server.
 *
 * @hints The ports to use for the new connection. This are just hints.
 *
 * @return A rtsp_ctrl instance. This is the structures that control the newly
 * created communication channel
 */
rtsp_ctrl *rtsp_init(nms_rtsp_hints * hints)
{
    rtsp_thread *rtsp_th;
    pthread_attr_t rtsp_attr;
    pthread_mutexattr_t mutex_attr;
    // pthread_condattr_t cond_attr;
    int n;

    // if ( !(rtsp_th = (rtsp_thread *) malloc(sizeof(rtsp_thread))) )
    // We use calloc so that we are not in need to initialize to zero below
    if (!(rtsp_th = (rtsp_thread *) calloc(1, sizeof(rtsp_thread)))) {
        nms_printf(NMSML_FATAL, "Could not alloc memory!\n");
        return NULL;
    }

    if ((n = pthread_mutexattr_init(&mutex_attr)) > 0)
        RET_ERR(NMSML_FATAL, "Could not init mutex attributes\n");

#if 0
#ifdef    _POSIX_THREAD_PROCESS_SHARED
    if ((n =
         pthread_mutexattr_setpshared(&mutex_attr,
                      PTHREAD_PROCESS_SHARED)) > 0)
        return NULL;
#endif
#endif
    if ((n = pthread_mutex_init(&(rtsp_th->comm_mutex), &mutex_attr)) > 0)
        RET_ERR(NMSML_FATAL, "Could not init mutex\n");

    /* // we give NULL to cond_init: uncommet this (and the declaration above)
     * if you want to give different attributes to cond
     if (pthread_condattr_init(&cond_attr) > 0)
     RET_ERR(NMSML_FATAL, "Could not init condition variable attributes\n");
     */
    if (pthread_cond_init(&(rtsp_th->cond_busy), NULL /*&cond_attr */ ) >
        0)
        RET_ERR(NMSML_FATAL, "Could not init condition variable\n");

    if ((rtsp_th->comm =
         (struct command *) malloc(sizeof(struct command))) == NULL)
        RET_ERR(NMSML_FATAL, "Could not alloc memory\n");

    nmst_init(&rtsp_th->transport);
    rtsp_th->default_rtp_proto = UDP;
    rtsp_th->status = INIT;
#if 0                // we do not need to initilize to zero because of calloc usage
    rtsp_th->descr_fmt = 0;
    memset(rtsp_th->waiting_for, '\0', sizeof(rtsp_th->waiting_for));
    rtsp_th->busy = 0;
    rtsp_th->urlname = NULL;
    (rtsp_th->in_buffer).size = 0;
    (rtsp_th->in_buffer).first_pkt_size = 0;
    (rtsp_th->in_buffer).data = NULL;
    rtsp_th->rtsp_queue = NULL;
    rtsp_th->force_rtp_port = 0;
#endif
    CC_ACCEPT_ALL(rtsp_th->accepted_CC);

    rtsp_th->hints = hints;
    // check for the exactness of values hinted
    if (hints) {        // hints given
        // set first RTP port
        if (hints->first_rtp_port > 0) {
            if (hints->first_rtp_port < RTSP_MIN_RTP_PORT)
                RET_ERR(NMSML_ERR,
                    "For security reasons RTSP Library imposes that port number should be greater than %d\n",
                    RTSP_MIN_RTP_PORT);
            else if (hints->first_rtp_port > 65535)
                RET_ERR(NMSML_ERR,
                    "Port number can't be greater than 65535\n");
            rtsp_th->force_rtp_port = hints->first_rtp_port;
            nms_printf(NMSML_WARN,
                   "RTP ports forced by user (not randomly generated)\n");
        }
        //force RTSP protocol
        switch (hints->pref_rtsp_proto) {
        case SOCK_NONE:
        case TCP:
            rtsp_th->transport.type = TCP;
            break;
#ifdef HAVE_LIBSCTP
        case SCTP:
            rtsp_th->transport.type = SCTP;
            break;
#endif
        default:
            RET_ERR(NMSML_ERR, "RTSP protocol not supported!\n");
        }
        //force RTP Protocol
        switch (hints->pref_rtp_proto) {
        case SOCK_NONE:
        case UDP:
            rtsp_th->default_rtp_proto = UDP;
            break;
        case TCP:
            if (rtsp_th->transport.type == TCP)
                rtsp_th->default_rtp_proto = TCP;
            else
                RET_ERR(NMSML_ERR, "RTP/RTSP protocols combination not supported!\n");
            break;
#ifdef HAVE_LIBSCTP
        case SCTP:
            if (rtsp_th->transport.type == SCTP)
                rtsp_th->default_rtp_proto = SCTP;
            else
                RET_ERR(NMSML_ERR, "RTP/RTSP protocols combination not supported!\n");
            break;
#endif
        default:
            RET_ERR(NMSML_ERR, "RTP protocol not supported!\n");
        }
    }
    // hook to rtp lib
    if (!(rtsp_th->rtp_th = rtp_init()))
        RET_ERR(NMSML_ERR, "Cannot initialize RTP structs\n");

    state_machine[0] = init_state;
    state_machine[1] = ready_state;
    state_machine[2] = playing_state;
    state_machine[3] = recording_state;

    // Creation of RTSP Thread
    pthread_attr_init(&rtsp_attr);
    if (pthread_attr_setdetachstate(&rtsp_attr, PTHREAD_CREATE_JOINABLE) !=
        0)
        RET_ERR(NMSML_FATAL, "Cannot set RTSP Thread attributes!\n");

    if ((n =
         pthread_create(&rtsp_th->rtsp_tid, NULL, &rtsp,
                (void *) rtsp_th)) > 0)
        RET_ERR(NMSML_FATAL, "Cannot create RTSP Thread: %s\n",
            strerror(n));

    return (rtsp_ctrl *) rtsp_th;
}

/**
 * Sends to the rtsp main loop the request to close the connection for the given control structure
 * @param rtsp_ctl The control structure for which to close the connection
 * @return 0
 */
int rtsp_close(rtsp_ctrl * rtsp_ctl)
{
    int got_error = 1;
    rtsp_thread * rtsp_th = (rtsp_thread*)rtsp_ctl;

    pthread_mutex_lock(&(rtsp_ctl->comm_mutex));
    rtsp_ctl->busy = 1;

    if (rtsp_th->status == INIT) {
        nms_printf(NMSML_NORM, BLANK_LINE);
        nms_printf(NMSML_NORM, "No Connection to close\n");
        goto quit_function;
    }

    get_curr_sess(GCS_INIT, rtsp_th);
    if (send_teardown_request(rtsp_th))
        goto quit_function;

    got_error = 0;

quit_function:
    if (got_error)
        rtsp_ctl->busy = 0;
    pthread_mutex_unlock(&(rtsp_ctl->comm_mutex));
    return got_error;
}

/**
 * Waits for the main loop to handle the last given command, this should be called after issuing a command
 * to the rtsp main loop.
 * @param rtsp_ctl The control structure for which to wait.
 * @return The last response received from the server
 */
int rtsp_wait(rtsp_ctrl * rtsp_ctl)
{
    rtsp_thread *rtsp_th = (rtsp_thread *) rtsp_ctl;

    pthread_mutex_lock(&(rtsp_th->comm_mutex));
    while (rtsp_th->busy)
        pthread_cond_wait(&(rtsp_th->cond_busy),
                  &(rtsp_th->comm_mutex));

    pthread_mutex_unlock(&(rtsp_th->comm_mutex));

    return rtsp_ctl->response_id;
}

/**
 * Gets the head of the RTP sessions queue linked to the given RTSP controller
 * @param rtsp_ctl The RTSP controller for which to get the RTP sessions queue
 * @return The RTP session queue
 */
inline rtp_session *rtsp_get_rtp_queue(rtsp_ctrl * rtsp_ctl)
{
    return ((rtsp_thread *) rtsp_ctl)->rtp_th->rtp_sess_head;
}

/**
 * Gets the RTP thread linked to the given RTSP controller
 * @param rtsp_ctl The RTSP controller for which to get the RTP thread
 * @return The RTP thread
 */
inline rtp_thread *rtsp_get_rtp_th(rtsp_ctrl * rtsp_ctl)
{
    return ((rtsp_thread *) rtsp_ctl)->rtp_th;
}

/**
 * Prints the informations about the given RTSP controller: 
 * Session informations, medium informations.
 * @param rtsp_ctl The controller for which to print the informations
 */
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
             (void*)str < (void*) &(sess->info->attr_list); str++)
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
                 (void*)str < (void*) &(med->medium_info->attr_list);
                 str++)
                if (*str)
                    nms_printf(NMSML_ALWAYS,
                           "\t* %s: %s\n",
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

/**
 * Checks if the given controller is waiting to process a given command
 * @param rtsp_ctl The controller for which to check the state
 * @return TRUE if its busy, FALSE if its ready
 */
inline int rtsp_is_busy(rtsp_ctrl * rtsp_ctl)
{
    return rtsp_ctl->busy;
}

/**
 * Sends to the controller the request to open a given url
 * @param rtsp_ctl The controller that should open the url
 * @param urlanem The path of the document to open
 * @return 0
 */
int rtsp_open(rtsp_ctrl * rtsp_ctl, char *urlname)
{
    int got_error = 1;
    char *server;
    rtsp_thread * rtsp_th = (rtsp_thread*)rtsp_ctl;

    if (!urlname || !*urlname) {
        nms_printf(NMSML_ERR, "No address given\n");
        goto quit_function;
    }

    pthread_mutex_lock(&(rtsp_ctl->comm_mutex));
    rtsp_ctl->busy = 1;

    if (rtsp_th->status != INIT) {
        nms_printf(NMSML_WARN, "Client already connected!\n");
        goto quit_function;
    }

    if (seturlname(rtsp_th, urlname) > 0)
        goto quit_function;

    urltokenize(rtsp_th->urlname, &server, NULL, NULL);
    if (server_connect
        (server, rtsp_th->server_port, &rtsp_th->transport.fd, rtsp_th->transport.type)) {
        rtsp_th->transport.fd = -1;
        nms_printf(NMSML_ERR, "Cannot connect to the server\n");
        goto quit_function;
    }

    free(server);
    if (send_get_request(rtsp_th))
        goto quit_function;

    got_error = 0;
   
quit_function:
    if (got_error)
        rtsp_ctl->busy = 0;
    pthread_mutex_unlock(&(rtsp_ctl->comm_mutex));
    return got_error;
}

/**
 * Sends to the controller the request to pause the current stream
 * @param rtsp_ctl The controller for which to pause the content
 * @return 0
 */
int rtsp_pause(rtsp_ctrl * rtsp_ctl)
{
    int got_error = 1;
    rtsp_thread * rtsp_th = (rtsp_thread*)rtsp_ctl;

    pthread_mutex_lock(&(rtsp_ctl->comm_mutex));
    rtsp_ctl->comm->arg[0] = '\0';
    rtsp_ctl->busy = 1;

    if (rtsp_th->status == INIT) {
        nms_printf(NMSML_ERR, "Player not initialized!\n");
        goto quit_function;
    }
    if (rtsp_th->status == READY) {
        nms_printf(NMSML_ERR,
               "I don't think you're yet playinq or recording\n");
        goto quit_function;
    }

    get_curr_sess(GCS_INIT, rtsp_th);
    if (send_pause_request(rtsp_th, rtsp_ctl->comm->arg)) 
        goto quit_function;

    got_error = 0;

quit_function:
    if (got_error)
        rtsp_ctl->busy = 0;
    pthread_mutex_unlock(&(rtsp_ctl->comm_mutex));
    return got_error;
}

/**
 * Sends to the controller the request to play the content in a given range
 * @param rtsp_ctl The controller that should start playing its content
 * @param start from where to start playing
 * @param stop where to stop playing
 * @return 0
 */
int rtsp_play(rtsp_ctrl * rtsp_ctl, double start, double stop)
{
    int got_error = 1;
    rtsp_thread * rtsp_th = (rtsp_thread*)rtsp_ctl;

    pthread_mutex_lock(&(rtsp_ctl->comm_mutex));

    if ((start >= 0) && (stop > 0))
        sprintf(rtsp_ctl->comm->arg, "npt=%.2f-%.2f", start, stop);
    else if (start >= 0)
        sprintf(rtsp_ctl->comm->arg, "npt=%.2f-", start);
    else if (stop > 0)
        sprintf(rtsp_ctl->comm->arg, "npt=-%.2f", stop);
    else
        *(rtsp_ctl->comm->arg) = '\0';

    rtsp_ctl->busy = 1;

    if (rtsp_th->status == INIT) {
        nms_printf(NMSML_ERR, "Player not initialized!\n");
        goto quit_function;
    }
    if (rtsp_th->status == RECORDING) {
        nms_printf(NMSML_ERR, "Still recording...\n");
        goto quit_function;
    }

    get_curr_sess(GCS_INIT, rtsp_th);
    if (send_play_request(rtsp_th, rtsp_ctl->comm->arg)) {
        goto quit_function;
    }

    got_error = 0;

quit_function:
    if (got_error)
        rtsp_ctl->busy = 0;
    pthread_mutex_unlock(&(rtsp_ctl->comm_mutex));
    return got_error;
}

/**
 * Starts playing the given new range of the stream, stopping the previously played one
 * This is implemented with a PAUSE and PLAY methods call
 * @param rtsp_ctl The controller that should start playing in the new range
 * @param start from where to start playing
 * @param stop where to stop playing
 * @return 0
 */
int rtsp_seek(rtsp_ctrl * rtsp_ctl, double new_start, double new_end)
{
    int got_error = 0;

    got_error = rtsp_pause(rtsp_ctl);
    if (!got_error) {
        rtsp_wait(rtsp_ctl);
        got_error = rtsp_play(rtsp_ctl, new_start, new_end);
    }

    return got_error;
}


/**
 * Sends to the controller the request to stop
 * @param rtsp_ctl The controller that should stop
 * @return 0
 */
int rtsp_stop(rtsp_ctrl * rtsp_ctl)
{
    int got_error = 1;
    rtsp_thread * rtsp_th = (rtsp_thread*)rtsp_ctl;

    pthread_mutex_lock(&(rtsp_ctl->comm_mutex));
    rtsp_ctl->comm->arg[0] = '\0';
    rtsp_ctl->busy = 1;

    if (rtsp_th->status == INIT) {
        nms_printf(NMSML_ERR, "Player not initialized!\n");
        goto quit_function;
    }
    if (rtsp_th->status == READY) {
        nms_printf(NMSML_ERR,
               "I don't think you're yet playing or recording\n");
        goto quit_function;
    }

    get_curr_sess(GCS_INIT, rtsp_th);
    if (send_pause_request(rtsp_th, rtsp_ctl->comm->arg)) {
        goto quit_function;
    }

    got_error = 0;

quit_function:
    if (got_error)
        rtsp_ctl->busy = 0;
    pthread_mutex_unlock(&(rtsp_ctl->comm_mutex));
    return got_error;
}

/**
 * Uninits the given RTSP controller
 * @param rtsp_ctl The controller to shut down
 * @return 0 If Shutdown was done
 * @return 1 If it wasn't possible to cancel the running main loop
 */
int rtsp_uninit(rtsp_ctrl * rtsp_ctl)
{
    void *ret = NULL;

    /* THREAD CANCEL */
    nms_printf(NMSML_DBG1, "Sending cancel signal to all threads\n");
    if (rtsp_ctl->rtsp_tid > 0) {
        nms_printf(NMSML_DBG1,
               "Sending cancel signal to RTSP Thread (ID: %lu)\n",
               rtsp_ctl->rtsp_tid);
        if (pthread_cancel(rtsp_ctl->rtsp_tid) != 0)
            nms_printf(NMSML_DBG2,
                   "Error while sending cancelation to RTSP Thread.\n");
        else
            pthread_join(rtsp_ctl->rtsp_tid, (void **) &ret);
        if (ret != PTHREAD_CANCELED) {
            nms_printf(NMSML_DBG2,
                   "Warning! RTSP Thread joined, but  not canceled!\n");
            return 1;
        }
    } else {
        nms_printf(NMSML_DBG1,
               "Cannot send cancel signal to RTSP Thread\n");
        return 1;
    }

    free(rtsp_ctl->comm);
    free(rtsp_ctl);

    return 0;
}
