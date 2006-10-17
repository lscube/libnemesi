/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
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

/*! \file rtsp.h
 * \brief \b rtsp library definitions.
 * */

/*! \defgroup RTSP RTSP Library
 *
 * \brief Real Time Streaming Protocol (RTSP) implementation - rfc 2326.
 *
 * The RTSP module requests the media streams and handles the service 
 * handshake. Once this phase is complete it behaves like a <em>remote 
 * controller</em> for the requested multimedia stream.
 *
 * @{ */

#ifndef __RTSP_H
#define __RTSP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>

#include <nemesi/sdp.h>
#include <nemesi/cc.h>
#include <nemesi/rtp.h>
#include <nemesi/rtcp.h>
#include <nemesi/wsocket.h>
#include <nemesi/utils.h>
#include <nemesi/comm.h>

/*! Default RTSP port (default setting from rfc) if not explicitly scecified. */
#define RTSP_DEFAULT_PORT 554

/*! Version of implemented protocol. */
#define RTSP_VER "RTSP/1.0"

/*! RTSP End of Line. */
#define RTSP_EL "\r\n"

/*! Lower bound for first RTP port if specified by user. */
#define RTSP_MIN_RTP_PORT 1024

typedef struct {
	int32 first_rtp_port;
	enum sock_types pref_rtsp_proto;
	enum sock_types pref_rtp_proto;
} nms_rtsp_hints;

typedef struct nms_rtsp_interleaved_s {
	int rtp_fd; //!< output rtp local socket 
	int rtcp_fd; //!< output rtcp local socket
	union {
		struct {
			uint8 rtp_ch;
			uint8 rtcp_ch;
		} tcp;
		struct {
			uint16 rtp_st;
			uint16 rtcp_st;
		} sctp;
	} proto;
	struct nms_rtsp_interleaved_s *next;
} nms_rtsp_interleaved;

/*! \enum Definition for possible states in RTSP state-machine
 * The last ("STATES_NUM") is used to know how many states are present in the machine.
 */
enum states { INIT, READY, PLAYING, RECORDING, STATES_NUM };
/*! \enum opcodes: enum which codify the available commands for the user.
* "COMMAND_NUM" is the total number of recognized commands.
* "NONE" is a special command, just for internal purpouses
* and not considered elsewhere.
*/
enum opcodes { OPEN, PLAY, PAUSE, STOP, CLOSE, COMMAND_NUM, NONE };

/*!
 * \brief Struct used for internal comunication
 *
 * */
struct command {
	enum opcodes opcode;	/*!< Command code inserted by user. */
	char arg[256];		/*!< Possible command arguments. */
};

/*!
 * \brief RTSP medium description.
 *
 * This struct is an item in a single-linked list that identifies all the media
 * that belong to a RTSP session. Through the \c rtp_sess pointer is possible
 * to get the RTP session of the described medium.
 
 * \note The methods SETUP e TEARDOWN are defined on a ``per medium'' scope,
 * they have to be sent for each medium and you must wait for a reply from 
 * the server for each of them.
 *
 * \see Medium_info
 * \see rtsp_session
 * */
typedef struct rtsp_medium_s {
	sdp_medium_info *medium_info;	/*!< Medium informations. */
	rtp_session *rtp_sess;	/*!< RTP session whom
				   the medium belongs */
	struct rtsp_medium_s *next;	/*!< Next medium. */
	char *filename;		/*!< Medium identifier. Used for the
				   ``per medium'' methods
				   (SETUP, TEARDOWN). */
} rtsp_medium;

/*!
 * \brief RTSP session description.
 *
 * Item of the RTSP sessions list.
 *
 * In this struct are present all the information used in a RTSP session.
 * For each session there is a queue of media that belongs to the same
 * ``presentation''
 *
 * \note The methods PLAY, PAUSE, RECORD are defined on a``per session'' scope,
 * they have to be sent for each active session. Aggregated media belongs to
 * the same session, so a session method will have effect to every of them.
 * E.g a PLAY method on an Audio Video session will start both audio and video
 * streams at the same time
 *
 * \see rtsp_session_info
 * \see rtsp_medium
 * */
typedef struct rtsp_session_s {
	uint64 Session_ID;	/*!< RTSP identifier. */
	int CSeq;		/*!< Last sent RTSP packet 
				   sequence number */
	char *pathname;		/*!< RTSP session identifier.
				   Used for the ``per session''
				   methods (PLAY, PAUSE, RECORD) */
	char *content_base;	/*!< Not \c NULL if a Content-Base field
				   is found in the response to the DESCRIBE 
				   method. In this case the field
				   <tt>\ref pathname</tt> and all the
				   <tt>\ref rtsp_medium</tt> of the session
				   are relative to \c content_base path. */
	sdp_session_info *info;	/*!< Session informations */

	rtsp_medium *media_queue;	/*!< Media queue */
	struct rtsp_session_s *next;	/*!< Next session */
	/********************************************/
	/* Do NOT USE IT! JUST FOR INTERNAL USAGE!  */
	/********************************************/
	char *body;		/*!< Contains the raw describe response.
				   It should be NEVER accessed directly.
				   All the data is available through
				   <tt>\ref rtsp_session_info</tt>. */
} rtsp_session;

/*!
 * \brief RTSP Packet buffer.
 *
 * There the packets read from the RTSP port are composed and stored.
 * Since it's possible that the message comes fragmented in many packets
 * from the underlying transport protocol, we first check for completion and
 * then the message will be processed.
 *
 * */
struct rtsp_buffer {
	size_t size;		/*!< Full buffer size. */
	size_t first_pkt_size;	/*!< First packet size. */
	char *data;		/*!< Raw data. */
};

#define RTSP_READY	0
#define RTSP_BUSY	1

/*!
 * \brief Definition of the common part for rtsp_thread and rtsp_ctrl structs
 */
#define RTSP_COMMON_IF \
			int pipefd[2]; \
			pthread_mutex_t comm_mutex; \
			struct command *comm; \
			enum states status;	/*!< Current RTSP state-machine status */ \
			unsigned char busy; /*!< Boolean value identifing if \
						the rtsp module is busy waiting reply from server*/ \
			pthread_t rtsp_tid; \
			char descr_fmt; /* Description format inside RTSP body */ \
			rtsp_session *rtsp_queue;/*!< List of active sessions. */ \
			cc_perm_mask accepted_CC;	/* accepted CC licenses */

/*!
 * \brief Main structure for the RTSP module.
 *
 * It contains the global data: the current state, the connection port,
 * the input buffer and the session queue.
 *
 * \note The \c type field shows the kind of media stream active, depending
 * on it RTSP has different behaviours.
 * 
 * \see rtsp_session
 * \see buffer
 * */
typedef struct {
	RTSP_COMMON_IF nms_rtsp_hints *hints;
	uint16 force_rtp_port;
	pthread_cond_t cond_busy;
	nms_transport transport;
	enum sock_types default_rtp_proto;
	nms_rtsp_interleaved *interleaved;
	uint16 next_ilvd_ch;
	// int fd; /*!< file descriptor for reading the data coming from the server */
	/*! \enum types enum possible kind of stream. */
	enum types { M_ON_DEMAND, CONTAINER } type;	/*!< Kind of active
							   media stream:
							   Media On Demand or
							   Container. */
	char waiting_for[64];	/*!< Expected response from server. */
	char *server_port;	/*!< Server listening port.
				 */
	char *urlname;		/*!< Requested URL */
	struct rtsp_buffer in_buffer;	/*!< Input buffer. */
	// rtsp_session *rtsp_queue;/*!< Active sessions. */
	rtp_thread *rtp_th;
} rtsp_thread;

typedef struct {
RTSP_COMMON_IF} rtsp_ctrl;

//******** interface functions ********************

	// old init function definitions: -|
	// /-------------------------------/
	// |- int init_rtsp(void);
	// \- struct rtsp_ctrl *init_rtsp(void);
rtsp_ctrl *rtsp_init(nms_rtsp_hints *);
inline int rtsp_is_busy(rtsp_ctrl *);
void rtsp_wait(rtsp_ctrl *);
int rtsp_close(rtsp_ctrl *);
int rtsp_open(rtsp_ctrl *, char *);
int rtsp_pause(rtsp_ctrl *);
int rtsp_stop(rtsp_ctrl *);
//int rtsp_play(rtsp_ctrl *, char *);
int rtsp_play(rtsp_ctrl *, double, double);
int rtsp_uninit(rtsp_ctrl *);
// enum states rtsp_status(rtsp_ctrl *);
#define rtsp_status(ctrl) ctrl->status
void rtsp_info_print(rtsp_ctrl *);
inline rtp_thread *rtsp_get_rtp_th(rtsp_ctrl * rtsp_ctl);
inline rtp_session *rtsp_get_rtp_queue(rtsp_ctrl * rtsp_ctl);
//
//***** ENDOF interface functions ******************

void rtsp_unbusy(rtsp_thread *);
int rtsp_reinit(rtsp_thread *);
void rtsp_clean(void *);

/*
extern int (*cmd[COMMAND_NUM]) (rtsp_thread *, char *);
*/
extern int (*cmd[COMMAND_NUM]) (rtsp_thread *, ...);

extern int (*state_machine[STATES_NUM]) (rtsp_thread *, short);

void *rtsp(void *);

int send_get_request(rtsp_thread *);
int send_pause_request(rtsp_thread *, char *);
int send_play_request(rtsp_thread *, char *);
int send_setup_request(rtsp_thread *);
int send_teardown_request(rtsp_thread *);

/*
int play_cmd(rtsp_thread *, char *);
int pause_cmd(rtsp_thread *, char *);
int stop_cmd(rtsp_thread *, char *);
int open_cmd(rtsp_thread *, char *);
int close_cmd(rtsp_thread *, char *);
*/

int play_cmd(rtsp_thread *, ...);
int pause_cmd(rtsp_thread *, ...);
int stop_cmd(rtsp_thread *, ...);
int open_cmd(rtsp_thread *, ...);
int close_cmd(rtsp_thread *, ...);

// RTSP packet handling/creation funcs.
int seturlname(rtsp_thread *, char *);
int handle_rtsp_pkt(rtsp_thread *);
int full_msg_rcvd(rtsp_thread *);
int rtsp_recv(rtsp_thread *);
int body_exists(char *);
int check_response(rtsp_thread *);
int check_status(char *, rtsp_thread *);
int set_transport_str(rtp_session *, char **);
int set_transport_str_udp(rtp_session *, char *);
int set_transport_str_tcp(rtp_session *, char *);
#ifdef HAVE_SCTP_NEMESI
int set_transport_str_sctp(rtp_session *, char *);
#endif
int get_transport_str(rtp_session *, char *);
int get_transport_str_udp(rtp_session *, char *, char *);
int get_transport_str_tcp(rtp_session *, char *, char *);
#ifdef HAVE_SCTP_NEMESI
int get_transport_str_sctp(rtp_session *, char *, char *);
#endif

#define GCS_INIT 0
#define GCS_NXT_SESS 1
#define GCS_NXT_MED 2
#define GCS_CUR_SESS 3
#define GCS_CUR_MED 4
#define GCS_UNINIT 5
void *get_curr_sess(int cmd, ...);
// int get_curr_sess(rtsp_thread *, rtsp_session **, rtsp_medium **);

int set_rtsp_sessions(rtsp_thread *, int, char *, char *);
int set_rtsp_media(rtsp_thread *);
rtsp_session *rtsp_sess_dup(rtsp_session *);
rtsp_session *rtsp_sess_create(char *, char *);
rtsp_medium *rtsp_med_create(int);
int remove_pkt(rtsp_thread *);

int init_state(rtsp_thread *, short);
int ready_state(rtsp_thread *, short);
int playing_state(rtsp_thread *, short);
int recording_state(rtsp_thread *, short);

int handle_get_response(rtsp_thread *);
int handle_setup_response(rtsp_thread *);
int handle_play_response(rtsp_thread *);
int handle_pause_response(rtsp_thread *);
int handle_teardown_response(rtsp_thread *);


#endif
/* @} */
