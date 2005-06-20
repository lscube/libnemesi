/* * 
 *  $Id$
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

/*! \file rtsp.h
 * \brief Header contenente le definizioni della libreria \b rtsp.
 * */

/*! \defgroup RTSP RTSP Library
 *
 * \brief Implementazione del protocollo Real Time Streaming Protocol (RTSP) -
 * rfc 2326.
 *
 * Il modulo RTSP si occupa della richiesta degli stream multimediali e della
 * negoziazione dei parametri con il server.  Una volta superata la fase
 * iniziale, si comporta come un <em>telecomando remoto</em> per il controllo
 * del flusso multimediale.
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
} NMSRtspHints;

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
 * \brief Struttura per la comunicazione dei comandi tra la ui e il modulo
 * RTSP.
 *
 * All'immissione di un comando da console, il modulo di interfaccia utente
 * compila questa struttura impostando il codice del comando e, eventualmente,
 * i relativi parametri.  Il modulo che implementa il protocollo RTSP si occupa
 * di interpretare il comando e avviare la sequenza di procedure adatte, le
 * quali possono essere semplicemente un comando da mandare al sever oppure una
 * serie di operazioni più lunghe, come succede ad esempio nel caso di apertura
 * della connessione e impostazione dei parametri, che comporta più di un
 * comando RTSP.
 *
 * */
struct command {
	enum opcodes opcode;	/*!< Command code inserted by user. */
	char arg[256];		/*!< Possible command arguments. */
};

/*!
 * \brief Struttura di descrizione di un medium RTSP.
 *
 * Tale struttura è un elemento di una lista single-linked che identificano
 * tutti i media appartenenti ad una sessione RTSP.  Attraverso questa
 * struttura, tramite il puntatore \c rtp_sess, si accede alla sessione RTP a
 * cui appartiene il medium descritto.
 *
 * \note I metodi SETUP e TEARDOWN sono definiti ``a livello medium'', vanno
 * cioè mandati per ognuno dei medium e si deve attendere una risposta dal
 * server per ognuno di essi.
 *
 * \see Medium_info
 * \see RTSP_Session
 * */
struct RTSP_Medium {
	SDP_Medium_info *medium_info;	/*!< Struttura delle informazioni
					  relative al medium. */
	struct RTP_Session *rtp_sess;	/*!< Puntatore alla struttura della
					  sessione RTP alla quale appartiene il
					  medium. */
	struct RTSP_Medium *next;	/*!< Puntatore al successivo medium
					  della sessione RTSP. */
	char *filename;			/*!< Identificare del medium. Utilizato
					  per i metodi ``a livello Medium''
					  (SETUP, TEARDOWN). */
};

/*!
 * \brief Struttura di descrizione della sessione RTSP.
 *
 * Elemento della lista delle sessioni RTSP.
 *
 * In questa struttura sono memorizzate tutte le informazioni utili per una
 * sessione RTSP.  Per ogni sessione c'è una coda di media che fanno parte
 * della stessa ``presentazione''.
 *
 * \note Imetodi PLAY, PAUSE, RECORD sono definiti ``a livello sessione'',
 * vanno cioè mandati per ogni sessione attiva. In caso di media aggregati,
 * essi faranno parte della stessa sessione, quindi un metodo spedito per una
 * sessione interesserà tutti i media ad essa appartenenti. Ciò è utilissimo,
 * ad esempio, nel caso di audio e video appartenenti ad un unico flusso e che
 * debbano essere sincronizzati.
 *
 * \see RTSP_Session_info
 * \see RTSP_Medium
 * */
struct RTSP_Session {
	uint64 Session_ID;	/*!< Identificatore della sessione RTSP. */
	int CSeq;		/*!< Numero di sequenza dell'ultimo pacchetto
				  RTSP spedito. */
	char *pathname;		/*!< Identificatore della sessione RTSP.
				  Utilizzato per i metodi ``a livello
				  sessione'' (PLAY, PAUSE, RECORD) */
	char *content_base;	/*!< Questo campo è diverso da \c NULL se è
				  stato trovato il campo Content-Base
				  nell'hearder del pacchetto di risposta al
				  metodo \em DESCRIBE. In tal caso il campo
				  <tt>\ref pathname</tt> della sessione e di
				  tutti gli <tt>\ref RTSP_Medium</tt> ad essa
				  appartenenti sono relativi el percorso
				  memorizzato in \c content_base. */
	SDP_Session_info *info;	/*!< Struttura delle informazioni
					  relative alla sessione */
	struct RTSP_Medium *media_queue;	/*!< Puntatore alla code dei
						  media appertenenti alla
						  sessione */
	struct RTSP_Session *next;	/*!< Puntatore alla sessione successiva
					  della coda */
	/*****************************/
	/* Da non usare. MAI         */
	/* SOLO PER INIZIALIZZAZIONE */
	/*****************************/
	char *body;		/*!< Puntatore alla zona di memoria dentro la
				  quale c'è il corpo della risposta alla
				  describe mandata dal server. Questa zona di
				  memoria non è MAI direttamente utilizzata. È
				  utilizzata solamente nella fase di settaggio
				  dei puntatori della struttura delle
				  informazioni come spiegato nella nota della
				  documentazione relativa alla struttura
				  <tt>\ref RTSP_Session_info</tt>. */
};

/*!
 * \brief Buffer di input per i pacchetti RTSP.
 *
 * Questa struttura rappresenta il buffer in cui vengono immagazzinati i
 * pacchetti letti sulla connessione RTSP.  Essendo RTSP un protocollo
 * testuale, è possibile che un suo pacchetto sia frammentato in più pacchetti
 * del livello di trasporto sottostante (TCP). Questo ci costringe a
 * controllare, prima di poterlo processare, che il pacchetto RTSP sia arrivato
 * completamente. In caso contrario si deve memorizzare il frammento arrivato e
 * aspettare che il resto arrivi successivamente.
 *
 * */
struct RTSP_buffer {
	int size;		/*!< Dimensione totale del buffer. */
	int first_pkt_size;	/*!< Dimensione del primo pacchetto. */
	char *data;		/*!< Puntatore alla zona dati. */
};

/*!
 * \brief Definition of the common part for RTSP_Thread and RTSP_Ctrl structs
 */
#define RTSP_COMMON_IF	int pipefd[2]; \
			pthread_mutex_t comm_mutex; \
			struct command *comm; \
			enum states status;	/*!< Current RTSP state-machine status */ \
			unsigned char busy; /*!< Boolean value identifing if \
						the rtsp module is busy waiting reply from server*/ \
			pthread_t rtsp_tid; \
			char descr_fmt; /* Description format inside RTSP body */ \
			struct RTSP_Session *rtsp_queue;/*!< List of active sessions. */

/*!
 * \brief Struttura al vertice della piramide del modulo RTSP.
 *
 * Questa struttura contiene i dati globali del modulo RTSP. Essi sono
 * principalmente lo stato corrente, la porta sulla quale è attiva la
 * connessione, il buffer di input dei pacchetti RTSP e la coda delle sessioni.
 *
 * \note È presente un campo che indica il tipo di flusso multimediale attivo.
 * Questo serve perché in base al tipo di presentazione supportata dal server
 * il protocollo RTSP assume comporatamenti diversi.
 * 
 * \see RTSP_Session
 * \see buffer
 * */
struct RTSP_Thread {
	RTSP_COMMON_IF
	NMSRtspHints *hints;
	uint16 force_rtp_port;
	int fd;		/*!< descrittore sul quale è attiva la connessione con
			  il server, dal quale cioè, verranno letti i dati
			  provenienti dal server */
		/*! \enum types enum dei possibili tipi di flussi. */
	enum types { M_ON_DEMAND, CONTAINER } type;	/*!< Tipo di flusso
							  multimediale attivo:
							  Media On Demand o
							  Container. */
	char waiting_for[64];	/*!< Stringa che contiene, eventualmente, la
				  descrizione della risposta che si sta
				  aspettando dal server. */
	char *server_port;	/*!< Porta sulla quale è in ascolto il server.
				 */
	char *urlname;		/*!< URL della richiesta. */
	struct RTSP_buffer in_buffer;	/*!< Buffer di input dei dati. */
	// struct RTSP_Session *rtsp_queue;/*!< Lista delle sessioni attive. */
};

struct RTSP_Ctrl {
	RTSP_COMMON_IF
};

/*
int (*cmd[COMMAND_NUM]) (struct RTSP_Thread *, char *);
*/
extern int (*cmd[COMMAND_NUM]) (struct RTSP_Thread *, ...);

extern int (*state_machine[STATES_NUM]) (struct RTSP_Thread *, short);

void *rtsp(void *);

	// old init function definitions: -|
	// /-------------------------------/
	// |- int init_rtsp(void);
	// \- struct RTSP_Ctrl *init_rtsp(void);
struct RTSP_Ctrl *rtsp_init(NMSRtspHints *);
int rtsp_reinit(struct RTSP_Thread *);
int create_thread(struct RTSP_Thread *);
void rtsp_clean(void *);
int rtsp_close(struct RTSP_Ctrl *);

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

int seturlname(struct RTSP_Thread *, char *);
int handle_rtsp_pkt(struct RTSP_Thread *);
int full_msg_rcvd( struct RTSP_Thread *);
int rtsp_recv(struct RTSP_Thread *);
int body_exists(char *);
int check_response(struct RTSP_Thread *);
int check_status(char *, struct RTSP_Thread *);
int set_transport_str(struct RTP_Session *, char **);
int get_transport_str(struct RTP_Session *, char *);

#define GCS_INIT 0
#define GCS_NXT_SESS 1
#define GCS_NXT_MED 2
#define GCS_CUR_SESS 3
#define GCS_CUR_MED 4
#define GCS_UNINIT 5
void *get_curr_sess(int cmd, ...);
// int get_curr_sess(struct RTSP_Thread *, struct RTSP_Session **, struct RTSP_Medium **);

int set_rtsp_sessions(struct RTSP_Thread *, int, char *, char *);
int set_rtsp_media(struct RTSP_Thread *);
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
/* @} */
