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

/**
 * @file rtsp_internals.h
 * @brief \b rtsp internal definitions.
 **/

#ifndef NEMESI_RTSP_INTERNALS_H
#define NEMESI_RTSP_INTERNALS_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <string.h>

#ifndef WIN32
#include <netdb.h>
#endif

#include "rtcp.h"
#include "utils.h"
#include "comm.h"
#include "rtsp.h"

/** @defgroup RTSP_layer RTSP Layer
 *
 * @brief Real Time Streaming Protocol (RTSP) implementation - rfc 2326.
 *
 * The RTSP module requests the media streams and handles the service 
 * handshake. Once this phase is complete it behaves like a <em>remote 
 * controller</em> for the requested multimedia stream.
 *
 * @{ 
 */

//RTSP commands tokens
#define SETUP_TKN       "SETUP"
#define REDIRECT_TKN    "REDIRECT"
#define PLAY_TKN        "PLAY"
#define PAUSE_TKN       "PAUSE"
#define SESSION_TKN     "SESSION"
#define RECORD_TKN      "RECORD"
#define EXT_METHOD_TKN  "EXT-"
#define HTTP_GET        "GET"    /* http get */    // not used

#define HELLO_TKN       "OPTIONS"
#define GET_TKN         "DESCRIBE"
#define GET_PARAM_TKN   "GET_PARAMETER"
#define SET_PARAM_TKN   "SET_PARAMETER"
#define CLOSE_TKN       "TEARDOWN"

/*
 * method response codes.  These are 100 greater than their
 * associated method values.  This allows for simplified
 * creation of event codes that get used in event_handler()
 */
#define RTSP_SETUP_RESPONSE         100
#define RTSP_GET_RESPONSE           101
#define RTSP_REDIRECT_RESPONSE      102
#define RTSP_PLAY_RESPONSE          103
#define RTSP_PAUSE_RESPONSE         104
#define RTSP_SESSION_RESPONSE       105
#define NEMESI_RTSP_HELLO_RESPONSE  106
#define RTSP_RECORD_RESPONSE        107
#define RTSP_CLOSE_RESPONSE         108
#define RTSP_GET_PARAM_RESPONSE     109
#define RTSP_SET_PARAM_RESPONSE     110
#define RTSP_EXTENSION_RESPONSE     111

#define DESCRIPTION_NONE_FORMAT     0
#define DESCRIPTION_SDP_FORMAT      1
#define DESCRIPTION_MH_FORMAT       2

//Codes not yet in NetEmbryo
#define RTSP_FOUND                  302

//RTSP Checks
#define RTSP_IS_SUCCESS(x) ((x>=200 /*RTSP_SUCCESS*/) && (x<300 /*RTSP_REDIRECT*/))? 1 : 0
#define RTSP_IS_REDIRECT(x) ((x>=300 /*RTSP_REDIRECT*/) && (x<400 /*RTSP_CLIENT_ERROR*/))? 1 : 0
#define RTSP_IS_CLIENT_ERROR(x) ((x>=400 /*RTSP_CLIENT_ERROR*/) && (x<500 /*RTSP_SERVER_ERROR*/))? 1 : 0
#define RTSP_IS_SERVER_ERROR(x) (x>=500 /*RTSP_SERVER_ERROR*/)? 1 : 0

#ifndef RTSP_BUFFERSIZE
#define RTSP_BUFFERSIZE 163840
#endif

typedef struct nms_rtsp_interleaved_s {
    int rtp_fd; //!< output rtp local socket 
    int rtcp_fd; //!< output rtcp local socket
    union {
        struct {
            uint8_t rtp_ch;
            uint8_t rtcp_ch;
        } tcp;
        struct {
            uint16_t rtp_st;
            uint16_t rtcp_st;
        } sctp;
    } proto;
    struct nms_rtsp_interleaved_s *next;
} nms_rtsp_interleaved;


/**
 * @brief Struct used for internal comunication
 *
 * */
struct command {
    char arg[256];        /*!< Possible command arguments. */
};

/**
 * @brief RTSP Packet buffer.
 *
 * There the packets read from the RTSP port are composed and stored.
 * Since it's possible that the message comes fragmented in many packets
 * from the underlying transport protocol, we first check for completion and
 * then the message will be processed.
 *
 **/
struct rtsp_buffer {
    size_t size;        /*!< Full buffer size. */
    size_t first_pkt_size;    /*!< First packet size. */
    char *data;        /*!< Raw data. */
};

/**
 * @brief Main structure for the RTSP module.
 *
 * It contains the global data: the current state, the connection port,
 * the input buffer and the session queue.
 *
 * @note The \c type field shows the kind of media stream active, depending
 * on it RTSP has different behaviours.
 * 
 * @see rtsp_session
 * @see buffer
 **/
typedef struct {
    RTSP_COMMON_IF nms_rtsp_hints *hints;
    uint16_t force_rtp_port;
    pthread_cond_t cond_busy;
    nms_transport transport;
    sock_type default_rtp_proto;
    nms_rtsp_interleaved *interleaved;
    uint16_t next_ilvd_ch;
    // int fd; /*!< file descriptor for reading the data coming from the server */
    /*! \enum types enum possible kind of stream. */
    enum types { M_ON_DEMAND, CONTAINER } type;    /*!< Kind of active
                               media stream:
                               Media On Demand or
                               Container. */
    char waiting_for[64];    /*!< Expected response from server. */
    char *server_port;    /*!< Server listening port.
                 */
    char *urlname;        /*!< Requested URL */
    struct rtsp_buffer in_buffer;    /*!< Input buffer. */
    // rtsp_session *rtsp_queue;/*!< Active sessions. */
    rtp_thread *rtp_th;
} rtsp_thread;

    // old init function definitions: -|
    // /-------------------------------/
    // |- int init_rtsp(void);
    // \- struct rtsp_ctrl *init_rtsp(void);

/** 
 * RTSP State Machine, dispatches incoming events to the
 * corrent handler.
 *
 * @defgroup RTSP_state_machine RTSP State Machine
 * @{ 
 */
extern int (*state_machine[STATES_NUM]) (rtsp_thread *, short);

void rtsp_unbusy(rtsp_thread *);
int rtsp_reinit(rtsp_thread *);
void rtsp_clean(void *);
void *rtsp(void *);

int init_state(rtsp_thread *, short);
int ready_state(rtsp_thread *, short);
int playing_state(rtsp_thread *, short);
int recording_state(rtsp_thread *, short);
/**
 * @}
 */

/** 
 * RTSP Response Handlers, they are used by the RTSP state machine
 * to handle incoming responses.
 *
 * @defgroup RTSP_handlers RTSP Response Handlers
 * @{ 
 */
int handle_rtsp_pkt(rtsp_thread *);
int handle_get_response(rtsp_thread *);
int handle_setup_response(rtsp_thread *);
int handle_play_response(rtsp_thread *);
int handle_pause_response(rtsp_thread *);
int handle_teardown_response(rtsp_thread *);
/**
 * @}
 */

/** 
 * RTSP Requests Senders, they are used by the RTSP public interface
 * to create and send the RTSP requests through the network
 *
 * @defgroup RTSP_send RTSP Requests Senders
 * @{ 
 */
int send_get_request(rtsp_thread *);
int send_pause_request(rtsp_thread *, char *);
int send_play_request(rtsp_thread *, char *);
int send_setup_request(rtsp_thread *);
int send_teardown_request(rtsp_thread *);
/**
 * @}
 */


/** 
 * RTSP Packets Handling, mainly called by the main loop to receive
 * RTSP packets and by handlers to parse them.
 *
 * @defgroup RTSP_internals RTSP Packets Handling
 * @{ 
 */
int full_msg_rcvd(rtsp_thread *);
int rtsp_recv(rtsp_thread *);
int body_exists(char *);
int check_response(rtsp_thread *);
int check_status(char *, rtsp_thread *);
int remove_pkt(rtsp_thread *);
int set_rtsp_media(rtsp_thread *);
/**
 * @}
 */

/** 
 * RTSP Session Management
 *
 * @defgroup RTSP_sessions RTSP Sessions Management
 * @{ 
 */
#define GCS_INIT 0
#define GCS_NXT_SESS 1
#define GCS_NXT_MED 2
#define GCS_CUR_SESS 3
#define GCS_CUR_MED 4
#define GCS_UNINIT 5

void *get_curr_sess(int cmd, ...);
int set_rtsp_sessions(rtsp_thread *, int, char *, char *);
rtsp_session *rtsp_sess_dup(rtsp_session *);
rtsp_session *rtsp_sess_create(char *, char *);
rtsp_medium *rtsp_med_create(rtsp_thread *);
/**
 * @}
 */

/** 
 * RTSP Transport Options
 *
 * @defgroup RTSP_transport RTSP Transport
 * @{ 
 */
int set_transport_str(rtp_session *, char **);
int set_transport_str_udp(rtp_session *, char *);
int set_transport_str_tcp(rtp_session *, char *);
#ifdef HAVE_LIBSCTP
int set_transport_str_sctp(rtp_session *, char *);
#endif
int get_transport_str(rtp_session *, char *);
int get_transport_str_udp(rtp_session *, char *, char *);
int get_transport_str_tcp(rtp_session *, char *, char *);
#ifdef HAVE_LIBSCTP
int get_transport_str_sctp(rtp_session *, char *, char *);
#endif
/**
 * @}
 */


#endif /* NEMESI_RTSP_INTERNALS_H */
/**
 * @}
 */
