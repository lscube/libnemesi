/* * 
 *  $Id:rtp.h 271 2006-01-20 18:45:51Z shawill $
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

#ifndef __RTP_H
#define __RTP_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <pthread.h>

#include <nemesi/comm.h>
#include <nemesi/bufferpool.h>
#include <nemesi/types.h>
#include <nemesi/utils.h>
#include <nemesi/wsocket.h>
#include <nemesi/rtpptdefs.h>

#define RTP_VERSION 2

// #define RTP_AVP_UDP "RTP/AVP/UDP"
#define RTP_AVP_UDP "RTP/AVP" // Default low-trasport is UDP. See RFC2326 12.39
#define RTP_SEQ_MOD (1<<16)
#define MIN_SEQUENTIAL 2
#define MAX_DROPOUT 3000
#define MAX_MISORDER 100

#define BANDWIDTH 16000 /* bytes-per-second */

#define RTP_OK			0
// general error codes. other, specific, error codes below
#define RTP_ERROR		1
#define RTP_ERRALLOC	-1

typedef struct {
#ifdef WORDS_BIGENDIAN
	uint32 ver:2;
	uint32 pad:1;
	uint32 ext:1;
	uint32 cc:4;
#else
	uint32 cc:4;	/* source count */
	uint32 ext:1;	/* extension flag */
	uint32 pad:1;	/* padding flag */
	uint32 ver:2;	/* version */
#endif
#ifdef WORDS_BIGENDIAN
	uint32 mark:1;
	uint32 pt:7;
#else
	uint32 pt:7;	/* payload type */
	uint32 mark:1;	/* marker flag */
#endif
	uint32 seq:16;	/* sequence number */
	uint32 time;	/* timestamp */
	uint32 ssrc;	/* synchronization source identifier */
	uint8 data[1];	/* beginning of RTP data */
} rtp_pkt;

typedef struct {
	uint32 len;
	uint32 timestamp;
	double time_sec;
	uint8 pt;
	char *data;
} rtp_frame;

#define RTP_PKT_CC(pkt)		(pkt->cc)
#define RTP_PKT_MARK(pkt)	(pkt->mark)
#define RTP_PKT_PT(pkt)		(pkt->pt)
#define RTP_PKT_SEQ(pkt)	ntohs(pkt->seq)
#define RTP_PKT_TS(pkt)		ntohl(pkt->time)
#define RTP_PKT_SSRC(pkt)	ntohl(pkt->ssrc)
#define RTP_PKT_DATA(pkt)	(pkt->data  + pkt->cc)
#define RTP_PAYLOAD_SIZE(pkt, pkt_len)	pkt_len - ((pkt->data)-(uint8 *)pkt) - pkt->cc - ((*(((uint8 *)pkt)+pkt_len-1)) * pkt->pad)
/*(pkt_len-sizeof(rtp_pkt)+1)  // note: sizeof(rtp_pkt) is size of rtp header + 1*/

#define RTP_TRANSPORT_SPEC			10
#define RTP_TRANSPORT_DELIVERY		20
#define RTP_TRANSPORT_SRCADDR		30
#define RTP_TRANSPORT_SRCADDRSTR	31
#define RTP_TRANSPORT_DSTADDR		40
#define RTP_TRANSPORT_DSTADDRSTR	41
#define RTP_TRANSPORT_LAYERS		50
#define RTP_TRANSPORT_MODE			60
#define RTP_TRANSPORT_APPEND		70
#define RTP_TRANSPORT_TTL			80
#define RTP_TRANSPORT_MCSRTP		90
#define RTP_TRANSPORT_MCSRTCP		91
#define RTP_TRANSPORT_MCSPORTS		92
#define RTP_TRANSPORT_CLIRTP		100
#define RTP_TRANSPORT_CLIRTCP		101
#define RTP_TRANSPORT_CLIPORTS		102
#define RTP_TRANSPORT_SRVRTP		110
#define RTP_TRANSPORT_SRVRTCP		111
#define RTP_TRANSPORT_SRVPORTS		112
#define RTP_TRANSPORT_SSRC			120

#define RTP_TRANSPORT_NOTSET		-1
#define RTP_TRANSPORT_SET			0

struct rtp_transport {
	char *spec;
	enum deliveries { unicast, multicast } delivery;
	nms_addr srcaddr; //!< stored in network order
	nms_addr dstaddr; //!< stored in network order
	int layers;
	enum modes { play, record } mode;
	int append;
	int ttl;
	in_port_t mcs_ports[2]; //!< stored in host order
	in_port_t cli_ports[2]; //!< stored in host order
	in_port_t srv_ports[2]; //!< stored in host order
	uint32 ssrc;
};

struct rtp_ssrc_stats {
	uint16 max_seq;		/* highest seq number seen */
	uint32 cycles;		/* shifted count of seq number cycles */
	uint32 base_seq;	/* base seq number */
	uint32 bad_seq;		/* last 'bad' seq number + 1 */
	uint32 probation;	/* sequ. pkts till source is valid */
	uint32 received;	/* RTP pkts received */
	uint32 expected_prior;	/* pkt expected at last interval */
	uint32 received_prior;	/* pkt received al last interval */
	uint32 transit;		/* relative trans time for prev pkt */
	double jitter;		/* extimated jitter */
	struct timeval lastrtp;	/* last RTP pkt reception time */
	struct timeval lastsr;	/* last RTCP SR pkt reception time */
	uint32 ntplastsr[2];	/* last RTCP SR pkt NTP reception time */
	uint32 firstts;		/* first pkt timestamp */
	struct timeval firsttv;	/* first pkt timeval */
};

struct rtp_ssrc_descr {
	char *end;
	char *cname;
	char *name;
	char *email;
	char *phone;
	char *loc;
	char *tool;
	char *note;
	char *priv;
};

struct rtp_session_stats {
	struct timeval tp;	/* the last time an RTCP pkt was transmitted */
	struct timeval tn;	/* the next scheduled transmission time of an RTCP pkt */
	uint32 pmembers;	/* the estimated number of session members at time tm was last recomputed */
	uint32 members;		/* the most currente estimate for the number of the session members */
	uint32 senders;		/* the most currente estimate for the number of senders in the session */
	double rtcp_bw;		/* the target RTCP bandwidht */
	uint8 we_sent;		/* flag that is true if the app has sent data since the second previous RTCP Report was transmitted */
	double avg_rtcp_size;	/* the average Compound RTCP pkt size, in octets, over all RTCP pkts sent and received by this partecipant */
	uint8 initial;		/* the flag that is true if the app has not yet sent an RTCP pkt */
};

#define SSRC_KNOWN	0
#define SSRC_NEW	1
#define SSRC_RTPNEW	2
#define SSRC_RTCPNEW	3
#define SSRC_COLLISION	4

typedef struct _rtp_ssrc {
	uint32 ssrc;
	nms_sockaddr rtp_from;
	nms_sockaddr rtcp_from;
	nms_sockaddr rtcp_to;
	int rtcptofd;
	struct rtp_ssrc_stats ssrc_stats;
	struct rtp_ssrc_descr ssrc_sdes;
	playout_buff po;
	struct _rtp_session *rtp_sess; // RTP session SSRC belogns to.
//	rtp_pt **rtpptdefs; // convenience pointer to the same struct as rtp_session. (not to be freed here)
	void *prsr_privs[128]; //!< I would like to keep rtp able to manage dimanic payolad changes at its best.
	struct _rtp_ssrc *next; // next known SSRC
	struct _rtp_ssrc *next_active; // next active SSRC
} rtp_ssrc;

struct rtp_conflict {
	nms_sockaddr transaddr;
	time_t time;
	struct rtp_conflict *next;
};

typedef int (*rtp_parser_init)(struct _rtp_session *rtp_sess, unsigned pt);
typedef int (*rtp_parser)(rtp_ssrc *stm_src, rtp_frame *fr);
typedef int (*rtp_parser_uninit)(rtp_ssrc *stm_src, unsigned pt);

typedef struct _rtp_session {
	uint32 local_ssrc;
	int rtpfd;
	int rtcpfd;
	struct rtp_transport transport;
	struct rtp_session_stats sess_stats;
	rtp_ssrc *ssrc_queue; // queue of all known SSRCs
	rtp_ssrc *active_ssrc_queue; // queue of active SSRCs
	struct rtp_conflict *conf_queue;
	buffer_pool bp;
	rtp_pt **rtpptdefs;
	rtp_parser *rtp_parsers;
	struct _rtp_session *next;
	pthread_mutex_t syn;
} rtp_session;

struct rtp_thread {
	rtp_session *rtp_sess_head;
	// struct timeval startime;
	pthread_mutex_t syn;
	pthread_t rtp_tid;
	pthread_t rtcp_tid;
};

enum rtp_protos {
	RTP,
	RTCP	
};

void *rtp(void *);

struct rtp_thread *rtp_init(void);
rtp_session *rtp_session_init(nms_sockaddr *, nms_sockaddr *);
int rtp_thread_create(struct rtp_thread *); // something like rtp_run could be better?

int rtp_recv(rtp_session *);
int rtp_hdr_val_chk(rtp_pkt *, int);
int rtp_ssrc_check(rtp_session *, uint32, rtp_ssrc **, nms_sockaddr *, enum rtp_protos);
int rtp_ssrc_init(rtp_session *, rtp_ssrc **, uint32, nms_sockaddr *,  enum rtp_protos);

#define RTP_FILL_OK			0
// rtp_fill error codes
#define RTP_BUFF_EMPTY		91
#define RTP_PARSE_ERROR		92
#define RTP_PKT_UNKNOWN 	93
#define RTP_IN_PRM_ERR		94
#define RTP_SSRC_NOTVALID	97
#define RTP_BUFFERING		99

#define RTP_PKT_DATA_LEN(pkt, len) (len > 0) ? len - ((uint8 *)(pkt->data)-(uint8 *)pkt) - pkt->cc - ((*(((uint8 *)pkt)+len-1)) * pkt->pad) : 0

// active ssrc list management
rtp_ssrc *rtp_active_ssrc_queue(rtp_session *rtp_sess_head);
rtp_ssrc *rtp_next_active_ssrc(rtp_ssrc *ssrc);

// wrappers for rtp_pkt
rtp_pkt *rtp_get_pkt(rtp_ssrc *, size_t *);
rtp_pkt *rtp_get_n_pkt(rtp_ssrc *, int *, uint32);
inline int rtp_rm_pkt(rtp_ssrc *);
int rtp_fill_buffer(rtp_ssrc *, rtp_frame *);
double rtp_get_next_ts(rtp_ssrc *);
int16 rtp_get_next_pt(rtp_ssrc *);

// time wrappers
int rtp_get_time(rtp_ssrc *ssrc, uint32 timestamp, struct timeval *time);

// rtp transport setup functions
int rtp_transport_set(rtp_session *, int, void *);
int rtp_transport_get(rtp_session *, int, void *, uint32);

// rtp transport wrapper functions for rtp_transport_get
inline char *rtp_transport_get_spec(rtp_session *);
inline enum deliveries rtp_transport_get_delivery(rtp_session *);
inline int rtp_transport_get_srcaddrstr(rtp_session *, char *, uint32);
inline nms_addr *rtp_transport_get_srcaddr(rtp_session *);
inline int rtp_transport_get_dstaddrstr(rtp_session *, char *, uint32);
inline nms_addr *rtp_transport_get_dstaddr(rtp_session *);
inline int rtp_transport_get_layers(rtp_session *);
inline enum modes rtp_transport_get_mode(rtp_session *);
inline int rtp_transport_get_append(rtp_session *);
inline int rtp_transport_get_ttl(rtp_session *);
inline int rtp_transport_get_mcsports(rtp_session *, in_port_t [2]);
inline in_port_t rtp_transport_get_mcsrtcpport(rtp_session *);
inline int rtp_transport_get_srvports(rtp_session *, in_port_t [2]);
inline in_port_t rtp_transport_get_srvrtcpport(rtp_session *);
inline int rtp_transport_get_cliports(rtp_session *, in_port_t [2]);
inline in_port_t rtp_transport_get_clirtcpport(rtp_session *);
inline uint32 rtp_transport_get_ssrc(rtp_session *);

// rtp transport wrapper functions for rtp_transport_set
// inline char *rtp_transport_set_spec(rtp_session *, char *); // not settable
inline int rtp_transport_set_delivery(rtp_session *, enum deliveries);
inline int rtp_transport_set_srcaddrstr(rtp_session *, char *);
inline int rtp_transport_set_srcaddr(rtp_session *, nms_addr *);
inline int rtp_transport_set_dstaddrstr(rtp_session *, char *);
inline int rtp_transport_set_dstaddr(rtp_session *, nms_addr *);
inline int rtp_transport_set_layers(rtp_session *, int);
inline int rtp_transport_set_mode(rtp_session *, enum modes);
inline int rtp_transport_set_append(rtp_session *, int);
inline int rtp_transport_set_ttl(rtp_session *, int);
inline int rtp_transport_set_mcsports(rtp_session *, in_port_t [2]);
inline int rtp_transport_set_mcsrtcpport(rtp_session *, in_port_t);
inline int rtp_transport_set_srvports(rtp_session *, in_port_t [2]);
inline int rtp_transport_set_srvrtcpport(rtp_session *, in_port_t);
inline int rtp_transport_set_cliports(rtp_session *, in_port_t [2]);
inline int rtp_transport_set_clirtcpport(rtp_session *, in_port_t);
inline int rtp_transport_set_ssrc(rtp_session *, uint32);

// internal functions

#define RTP_PRSR_ERROR		-1
#define RTP_DST_TOO_SMALL	-2
#define RTP_REG_STATIC		-3

void rtp_parsers_init(void);
int rtp_parser_reg(rtp_parser parsers_defs[], int16, char *);
rtp_parser *rtp_parsers_new(void);

// rtcp connection functions
int rtcp_to_connect(rtp_ssrc *, nms_addr *, in_port_t);

// SSRC management functions
void rtp_init_seq(rtp_ssrc *, uint16);
void rtp_update_seq(rtp_ssrc *, uint16);

void rtp_clean(void *);
#endif
