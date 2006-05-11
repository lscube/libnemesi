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

#include <config.h>

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
} nms_rtsp_hints;

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
struct rtsp_medium {
	sdp_medium_info *medium_info;	/*!< Medium informations. */
	struct rtp_session *rtp_sess;	/*!< RTP session whom
					  the medium belongs */
	struct rtsp_medium *next;	/*!< Next medium. */
	char *filename;			/*!< Medium identifier. Used for the
					  ``per medium'' methods
					  (SETUP, TEARDOWN). */
};

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
struct rtsp_session {
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
	
	struct rtsp_medium *media_queue; /*!< Media queue */
	struct rtsp_session *next;	/*!< Next session */
	/********************************************/
	/* Do NOT USE IT! JUST FOR INTERNAL USAGE!  */
	/********************************************/
	char *body;		/*!< Contains the raw describe response.
				  It should be NEVER accessed directly.
				  All the data is available through
				  <tt>\ref rtsp_session_info</tt>. */
};

/*!
 * \brief RTSP Packet buffer.
 *
 * There the packets read on the RTSP port are composed and stored.
 * Since it's possible that the message comes fragmented in many packets
 * from the underlying transport protocol, we first check for completion and
 * then the message will be processed.
 *
 * */
struct rtsp_buffer {
	int size;		/*!< Full buffer size. */
	int first_pkt_size;	/*!< First packet size. */
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
			struct rtsp_session *rtsp_queue;/*!< List of active sessions. */ \
			cc_perm_mask accepted_CC; /* accepted CC licenses */

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
struct rtsp_thread {
	RTSP_COMMON_IF
	nms_rtsp_hints *hints;
	uint16 force_rtp_port;
	pthread_cond_t cond_busy;
	int fd;		/*!< file descriptor for reading the data coming 
			  from the server */
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
	// struct rtsp_session *rtsp_queue;/*!< Active sessions. */
	struct rtp_thread *rtp_th;
};

struct rtsp_ctrl {
	RTSP_COMMON_IF
};

//******** interface functions ********************

	// old init function definitions: -|
	// /-------------------------------/
	// |- int init_rtsp(void);
	// \- struct rtsp_ctrl *init_rtsp(void);
struct rtsp_ctrl *rtsp_init(nms_rtsp_hints *);
inline int rtsp_is_busy(struct rtsp_ctrl *);
void rtsp_wait(struct rtsp_ctrl *);
int rtsp_close(struct rtsp_ctrl *);
int rtsp_open(struct rtsp_ctrl *, char *);
int rtsp_pause(struct rtsp_ctrl *);
int rtsp_stop(struct rtsp_ctrl *);
//int rtsp_play(struct rtsp_ctrl *, char *);
int rtsp_play(struct rtsp_ctrl *, double, double);
int rtsp_uninit(struct rtsp_ctrl *);
// enum states rtsp_status(struct rtsp_ctrl *);
#define rtsp_status(ctrl) ctrl->status
void rtsp_info_print(struct rtsp_ctrl *);
//
//***** ENDOF interface functions ******************

void rtsp_unbusy(struct rtsp_thread *);
int rtsp_reinit(struct rtsp_thread *);
void rtsp_clean(void *);

/*
extern int (*cmd[COMMAND_NUM]) (struct rtsp_thread *, char *);
*/
extern int (*cmd[COMMAND_NUM]) (struct rtsp_thread *, ...);

extern int (*state_machine[STATES_NUM]) (struct rtsp_thread *, short);

void *rtsp(void *);

int send_get_request(struct rtsp_thread *);
int send_pause_request(struct rtsp_thread *, char *);
int send_play_request(struct rtsp_thread *, char *);
int send_setup_request(struct rtsp_thread *);
int send_teardown_request(struct rtsp_thread *);

/*
int play_cmd(struct rtsp_thread *, char *);
int pause_cmd(struct rtsp_thread *, char *);
int stop_cmd(struct rtsp_thread *, char *);
int open_cmd(struct rtsp_thread *, char *);
int close_cmd(struct rtsp_thread *, char *);
*/

int play_cmd(struct rtsp_thread *, ...);
int pause_cmd(struct rtsp_thread *, ...);
int stop_cmd(struct rtsp_thread *, ...);
int open_cmd(struct rtsp_thread *, ...);
int close_cmd(struct rtsp_thread *, ...);

// RTSP packet handling/creation funcs.
int seturlname(struct rtsp_thread *, char *);
int handle_rtsp_pkt(struct rtsp_thread *);
int full_msg_rcvd( struct rtsp_thread *);
int rtsp_recv(struct rtsp_thread *);
int body_exists(char *);
int check_response(struct rtsp_thread *);
int check_status(char *, struct rtsp_thread *);
int set_transport_str(struct rtp_session *, char **);
int get_transport_str(struct rtp_session *, char *);

#define GCS_INIT 0
#define GCS_NXT_SESS 1
#define GCS_NXT_MED 2
#define GCS_CUR_SESS 3
#define GCS_CUR_MED 4
#define GCS_UNINIT 5
void *get_curr_sess(int cmd, ...);
// int get_curr_sess(struct rtsp_thread *, struct rtsp_session **, struct rtsp_medium **);

int set_rtsp_sessions(struct rtsp_thread *, int, char *, char *);
int set_rtsp_media(struct rtsp_thread *);
struct rtsp_session *rtsp_sess_dup(struct rtsp_session *);
struct rtsp_session *rtsp_sess_create(char *, char *);
struct rtsp_medium *rtsp_med_create(int);
int remove_pkt(struct rtsp_thread *);

int init_state(struct rtsp_thread *, short);
int ready_state(struct rtsp_thread *, short);
int playing_state(struct rtsp_thread *, short);
int recording_state(struct rtsp_thread *, short);

int handle_get_response(struct rtsp_thread *);
int handle_setup_response(struct rtsp_thread *);
int handle_play_response(struct rtsp_thread *);
int handle_pause_response(struct rtsp_thread *);
int handle_teardown_response(struct rtsp_thread *);


#endif
/* @} */
