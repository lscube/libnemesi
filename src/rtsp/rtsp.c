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
#include "utils/utils.h"
#include <fcntl.h>
#include "nemesi/version.h"
#include "nemesi/bufferpool.h"
#include <netembryo/wsocket-internal.h>

/** @file rtsp.c
 * This file contains the interface functions to the rtsp packets and requests handling of the library
 */

#define RET_ERR(ret_level, ...)    do { \
                    nms_printf(ret_level, __VA_ARGS__ ); \
                    free(rtsp_th->comm); \
                    free(rtsp_th); \
                    return NULL; \
                } while (0)

RTSP_Error const RTSP_Ready = { {0, "Nemesi Library is in Initialized state"}, 0 };
RTSP_Error const RTSP_Reinitialized = { {-1, "Nemesi Library has been reinitialized"}, 1 };

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
#ifdef WIN32
    WSADATA wsaData;
#endif

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

#ifdef WIN32
    if ( WSAStartup(0x0202, &wsaData) ) {
         nms_printf(NMSML_FATAL, "Could not initialize windows sockets!\n");
         return NULL;
    }
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

    CC_ACCEPT_ALL(rtsp_th->accepted_CC);

    // hook to rtp lib
    if (!(rtsp_th->rtp_th = rtp_init()))
        RET_ERR(NMSML_ERR, "Cannot initialize RTP structs\n");

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
        // prebuffer size
        if (hints->prebuffer_size > 0)
            rtsp_th->rtp_th->prebuffer_size = hints->prebuffer_size;

        //force RTSP protocol
        switch (hints->pref_rtsp_proto) {
        case SOCK_NONE:
        case TCP:
            rtsp_th->transport.sock.socktype = TCP;
            break;
#ifdef HAVE_SCTP
        case SCTP:
            rtsp_th->transport.sock.socktype = SCTP;
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
            if (rtsp_th->transport.sock.socktype == TCP)
                rtsp_th->default_rtp_proto = TCP;
            else
                RET_ERR(NMSML_ERR, "RTP/RTSP protocols combination not supported!\n");
            break;
#ifdef HAVE_SCTP
        case SCTP:
            if (rtsp_th->transport.sock.socktype == SCTP)
                rtsp_th->default_rtp_proto = SCTP;
            else
                RET_ERR(NMSML_ERR, "RTP/RTSP protocols combination not supported!\n");
            break;
#endif
        default:
            RET_ERR(NMSML_ERR, "RTP protocol not supported!\n");
        }
    }

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
 * @return The last response received from the server if the server is still alive.
 *          RTSP_Ready If the library has just been initialized
 *          RTSP_Reinitialized If the library has been reinitialized (connection error, server went down, etc)
 */
RTSP_Error rtsp_wait(rtsp_ctrl * rtsp_ctl)
{
    rtsp_thread *rtsp_th = (rtsp_thread *) rtsp_ctl;

    pthread_mutex_lock(&(rtsp_th->comm_mutex));
    while (rtsp_th->busy)
        pthread_cond_wait(&(rtsp_th->cond_busy),
                  &(rtsp_th->comm_mutex));

    pthread_mutex_unlock(&(rtsp_th->comm_mutex));

    if (rtsp_ctl->response_id == 0)
        return RTSP_Ready;
    else if (rtsp_ctl->response_id == -1)
        return RTSP_Reinitialized;
    else
        return *get_RTSP_Error(rtsp_ctl->response_id);
}

/**
 * Gets the head of the RTP sessions queue linked to the given RTSP controller
 * @param rtsp_ctl The RTSP controller for which to get the RTP sessions queue
 * @return The RTP session queue
 */
rtp_session *rtsp_get_rtp_queue(rtsp_ctrl * rtsp_ctl)
{
    return ((rtsp_thread *) rtsp_ctl)->rtp_th->rtp_sess_head;
}

/**
 * Gets the RTP thread linked to the given RTSP controller
 * @param rtsp_ctl The RTSP controller for which to get the RTP thread
 * @return The RTP thread
 */
rtp_thread *rtsp_get_rtp_th(rtsp_ctrl * rtsp_ctl)
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
            nms_printf(NMSML_ALWAYS, "%s %s\n", attr->name, attr->value);
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
                nms_printf(NMSML_ALWAYS, "\t* %s %s\n",
                           attr->name, attr->value);
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
int rtsp_is_busy(rtsp_ctrl * rtsp_ctl)
{
    return rtsp_ctl->busy;
}
#if 0
/**
 * Support function for rtsp_open that connects to the given host and port and gives a new file descriptor
 * @param server string with the server address.
 * @param port which port to connect
 * @param sock where to save the new file descriptor
 * @return 1 if connected, 0 if something failed
 * @see tcp_open
 */
static int server_connect(char *host, char *port, int *sock, sock_type sock_type)
{
    int n, connect_new;
    struct addrinfo *res, *ressave;
    struct addrinfo hints;
#ifdef HAVE_SCTP
    struct sctp_initmsg initparams;
    struct sctp_event_subscribe subscribe;
#endif

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_flags = AI_CANONNAME;
#ifdef IPV6
    hints.ai_family = AF_UNSPEC;
#else
    hints.ai_family = AF_INET;
#endif

    switch (sock_type) {
    case SCTP:
#ifndef HAVE_SCTP
        return nms_printf(NMSML_ERR,
                  "%s: SCTP protocol not compiled in\n",
                  PROG_NAME);
        break;
#endif    // else go down to TCP case (SCTP and TCP are both SOCK_STREAM type)
    case TCP:
        hints.ai_socktype = SOCK_STREAM;
        break;
    case UDP:
        hints.ai_socktype = SOCK_DGRAM;
        break;
    default:
        return nms_printf(NMSML_ERR,
                  "%s: Unknown socket type specified\n",
                  PROG_NAME);
        break;
    }

    if ((n = gethostinfo(&res, host, port, &hints)) != 0) {
       fprintf(stderr, "N: %u\n", n);
        return nms_printf(NMSML_ERR, "%s: %s\n", PROG_NAME,
                  gai_strerror(n));
       }

    ressave = res;

    connect_new = (*sock < 0);

    do {
#ifdef HAVE_SCTP
        if (sock_type == SCTP)
            res->ai_protocol = IPPROTO_SCTP;
#endif // TODO: remove this code when SCTP will be supported from getaddrinfo()

        if (connect_new && (*sock =
             socket(res->ai_family, res->ai_socktype,
                res->ai_protocol)) < 0)
            continue;

#ifdef HAVE_SCTP
        if (sock_type == SCTP) {
            // Enable the propagation of packets headers
            memset(&subscribe, 0, sizeof(subscribe));
            subscribe.sctp_data_io_event = 1;
            if (setsockopt(*sock, SOL_SCTP, SCTP_EVENTS, &subscribe,
                    sizeof(subscribe)) < 0)
                return nms_printf(NMSML_ERR, "setsockopts(SCTP_EVENTS) error in sctp_open.\n");

            // Setup number of streams to be used for SCTP connection
            memset(&initparams, 0, sizeof(initparams));
            initparams.sinit_max_instreams = MAX_SCTP_STREAMS;
            initparams.sinit_num_ostreams = MAX_SCTP_STREAMS;
            if (setsockopt(*sock, SOL_SCTP, SCTP_INITMSG, &initparams,
                    sizeof(initparams)) < 0)
                return nms_printf(NMSML_ERR, "setsockopts(SCTP_INITMSG) error in sctp_open.\n");
        }
#endif

        if (connect(*sock, res->ai_addr, res->ai_addrlen) == 0)
            break;
        if (connect_new) {
            if (close(*sock) < 0)
                return nms_printf(NMSML_ERR, "(%s) %s", PROG_NAME,
                          strerror(errno));
            else
                *sock = -1;
        }

    } while ((res = res->ai_next) != NULL);

    freeaddrinfo(ressave);

    if (!res)
        return nms_printf(NMSML_ERR,
                  "Server connect error for \"%s:%s\"", host,
                  port);

    return 0;
}
#endif
static int seturlname(rtsp_thread * rtsp_th, char *urlname)
{
    char *server = NULL, *port = NULL, *path = NULL;

    if (urltokenize(urlname, &server, &port, &path) > 0) {
        return 1;
    }
    if (port == NULL) {
        if ((port = (char *) malloc(6))) {
            snprintf(port, 6, "%d", RTSP_DEFAULT_PORT);
        } else {
            return 1;
        }
    }
    if (path == NULL) {
        if ((path = (char *) calloc(1, 1)) == NULL) {
            return 1;
        }
    }
    nms_printf(NMSML_DBG1, "server %s port %s\n", server, port);

    if ((rtsp_th->urlname = malloc(strlen("rtsp://") + strlen(server) + 1 +
                            strlen(path) + 1)) == NULL)
        return 1;

    strcpy(rtsp_th->urlname, "rtsp://");
    strcat(rtsp_th->urlname, server);
    strcat(rtsp_th->urlname, path);

    rtsp_th->server_port = strdup(port);

    // free no more useful memory
    free(server);
    free(port);
    free(path);

    return 0;
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
#warning Finish port to netembryo
    if (sock_connect
        (server, rtsp_th->server_port, &rtsp_th->transport.sock.fd, rtsp_th->transport.sock.socktype)) {
        rtsp_th->transport.sock.fd = -1;
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
    int lib_error;
    RTSP_Error rtsp_error;

    lib_error = rtsp_pause(rtsp_ctl);
    if (!lib_error) {
        rtsp_error = rtsp_wait(rtsp_ctl);
        if (rtsp_error.message.reply_code == 200) {
            rtp_ssrc *ssrc = NULL;
            for (ssrc = rtp_active_ssrc_queue(rtsp_get_rtp_queue(rtsp_ctl));
                 ssrc;
                 ssrc = rtp_next_active_ssrc(ssrc)) {
                ssrc->done_seek = 1;
                rtp_rm_all_pkts(ssrc);
            }
            lib_error = rtsp_play(rtsp_ctl, new_start, new_end);
        }
        else
            lib_error = 1;
    }

    return lib_error;
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
    #warning pthread_t should be an opaque structure
    if ( thread_isvalid(rtsp_ctl->rtsp_tid) ) {
        nms_printf(NMSML_DBG1,
               "Sending cancel signal to RTSP Thread (ID: %lu)\n",
               thread_getuid(rtsp_ctl->rtsp_tid));
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

#ifdef WIN32
    WSACleanup();
#endif

    return 0;
}
