/* * 
 *  ./include/nemesi/rtsp.h: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
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

#ifndef __RTSP_H
#define __RTSP_H

#include <config.h>

#include <stdio.h>
#include <netdb.h>
#include <pthread.h>
#include <string.h>

#include <nemesi/main.h>
#include <nemesi/rtp.h>
#include <nemesi/rtcp.h>
#include <nemesi/wsocket.h>
#include <nemesi/utils.h>
#include <nemesi/comm.h>

#define RTSP_DEFAULT_PORT 554

#define RTSP_VER  "RTSP/1.0"

#define COMMAND_NUM 5
#define STATE_NUM 4

struct command {
	enum opcodes { OPEN, PLAY, PAUSE, STOP, CLOSE } opcode;
	char arg[256];
};

struct attr {
	struct attr *next;
	char *a;
};

struct Medium_info {
	char *m;		/* formato multimediale ed indirizzo di trasporto */
	char *i;		/* titolo del media */
	char *c;		/* informazioni di connessione */
	char *b;		/* informazioni sulla banda */
	char *k;		/* chiave di codifica */
	struct attr *attr_list;	/* attributi */
};

struct RTSP_Session_info {
	char *v;		/* versione protocollo */
	char *o;		/* creatore e identificazione di sessione */
	char *s;		/* nome della sessione */
	char *i;		/* informazioni sulla sessione */
	char *u;		/* descrizione dell'URI */
	char *e;		/* indirizzo e-mail */
	char *p;		/* numero telefonico */
	char *c;		/* informazioni sulla connessione */
	char *b;		/* informazioni sulla banda */

	char *t;		/* tempo in cui la sessione e' attiva */
	char *r;		/* ripetizioni temporali */

	char *z;		/* cambio di fuso orario */
	char *k;		/* chiave di codifica */

	struct attr *attr_list;	/* attributi */
};

struct RTSP_Medium {
	struct Medium_info medium_info;
	struct RTP_Session *rtp_sess;
	struct RTSP_Medium *next;
	char *filename;
};

struct RTSP_Session {
	uint64 Session_ID;
	int CSeq;
	char *pathname;
	char *content_base;
	struct RTSP_Session_info info;
	struct RTSP_Medium *media_queue;
	struct RTSP_Session *next;
	/*****************************/
	/* Da non usare. MAI         */
	/* SOLO PER INIZIALIZZAZIONE */
	/*****************************/
	char *body;
};

struct buffer {
	int size;
	int first_pkt_size;
	char *data;
};

struct RTSP_Thread {
	int fd;
	enum types { M_ON_DEMAND, CONTAINER } type;
	enum stati { INIT, READY, PLAYING, RECORDING } status;
	unsigned char busy;
	char waiting_for[64];
	char *server_port;
	char *urlname;
	struct buffer in_buffer;
	struct RTSP_Session *rtsp_queue;
};

/*
int (*cmd[COMMAND_NUM]) (struct RTSP_Thread *, char *);
*/
int (*cmd[COMMAND_NUM]) (struct RTSP_Thread *, ...);

int (*state_machine[STATE_NUM]) (struct RTSP_Thread *, short);

void *rtsp(void *);

int init_rtsp(void);
int reinit_rtsp(struct RTSP_Thread *);
int create_thread(struct RTSP_Thread *);
void rtsp_clean(void *);

int send_get_request(struct RTSP_Thread *);
int send_pause_request(struct RTSP_Thread *, char *);
int send_play_request(struct RTSP_Thread *, char *);
int send_setup_request(struct RTSP_Thread *);
int send_teardown_request(struct RTSP_Thread *);

/*
int play_cmd(struct RTSP_Thread *, char *);
int pause_cmd(struct RTSP_Thread *, char *);
int stop_cmd(struct RTSP_Thread *, char *);
int open_cmd(struct RTSP_Thread *, char *);
int close_cmd(struct RTSP_Thread *, char *);
*/

int play_cmd(struct RTSP_Thread *, ...);
int pause_cmd(struct RTSP_Thread *, ...);
int stop_cmd(struct RTSP_Thread *, ...);
int open_cmd(struct RTSP_Thread *, ...);
int close_cmd(struct RTSP_Thread *, ...);

int handle_rtsp_pkt(struct RTSP_Thread *);
int full_msg_rcvd( struct RTSP_Thread *);
int rtsp_recv(struct RTSP_Thread *);
int body_exists(char *);
int check_response(struct RTSP_Thread *);
int check_status(char *, struct RTSP_Thread *);
int set_transport_str(struct RTP_Session *, char **);
int get_transport_str(struct RTP_Session *, char *);
int get_curr_sess(struct RTSP_Thread *, struct RTSP_Session **, struct RTSP_Medium **);
int set_rtsp_sessions(struct RTSP_Thread *, int, char *, char *, char);
int set_rtsp_media(struct RTSP_Thread *, int , char **);
struct RTSP_Session *rtsp_sess_copy(struct RTSP_Session *);
struct RTSP_Session *rtsp_sess_create(char *, char *);
struct RTSP_Medium *rtsp_med_create(int);
int remove_pkt(struct RTSP_Thread *);

int init_state(struct RTSP_Thread *, short);
int ready_state(struct RTSP_Thread *, short);
int playing_state(struct RTSP_Thread *, short);
int recording_state(struct RTSP_Thread *, short);

int handle_get_response(struct RTSP_Thread *);
int handle_setup_response(struct RTSP_Thread *);
int handle_play_response(struct RTSP_Thread *);
int handle_pause_response(struct RTSP_Thread *);
int handle_teardown_response(struct RTSP_Thread *);


#endif
