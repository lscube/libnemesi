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
 * @file rtp.h
 * RTP layer public header
 */

#ifndef NEMESI_RTP_H
#define NEMESI_RTP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
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
#include <stdint.h>

#include "comm.h"
#include "utils.h"
#include "transport.h"
#include "rtpptdefs.h"

#define RTP_VERSION 2

// #define RTP_AVP_UDP "RTP/AVP/UDP"
#define RTP_AVP_UDP  "RTP/AVP"      //!< Default low-trasport is UDP. See RFC2326 12.39
#define RTP_AVP_TCP  "RTP/AVP/TCP"  //!< interleaved TCP low transport.
#define RTP_AVP_SCTP "RTP/AVP/SCTP" //!< multistream SCTP low transport.

#define RTP_SEQ_MOD (1<<16)
#define MIN_SEQUENTIAL 2
#define MAX_DROPOUT 3000
#define MAX_MISORDER 100

#define BANDWIDTH 16000     //!< bytes-per-second

#define RTP_OK           0
// general error codes. other, specific, error codes below
#define RTP_ERROR        1
#define RTP_ERRALLOC    -1


/**
 * RTP packet structure, the header is a bitfield
 */
typedef struct {
#ifdef WORDS_BIGENDIAN
    uint32_t ver:2;
    uint32_t pad:1;
    uint32_t ext:1;
    uint32_t cc:4;
#else
    uint32_t cc:4;        //!< source count
    uint32_t ext:1;       //!< extension flag
    uint32_t pad:1;       //!< padding flag
    uint32_t ver:2;       //!< version
#endif
#ifdef WORDS_BIGENDIAN
    uint32_t mark:1;
    uint32_t pt:7;
#else
    uint32_t pt:7;        //!< payload type
    uint32_t mark:1;      //!< marker flag 
#endif
    uint32_t seq:16;      //!< sequence number
    uint32_t time;        //!< timestamp
    uint32_t ssrc;        //!< synchronization source identifier
    uint8_t data[1];      //!< beginning of RTP data
} rtp_pkt;

typedef struct {
    uint32_t len;
    char *data;
} rtp_buff;


typedef struct {
    long len;
    uint32_t timestamp;
    double time_sec;
    int fps;
    uint8_t pt;
    char *data;
} rtp_frame;

#define RTP_PKT_CC(pkt)     (pkt->cc)
#define RTP_PKT_MARK(pkt)   (pkt->mark)
#define RTP_PKT_PT(pkt)     (pkt->pt)
#define RTP_PKT_SEQ(pkt)    ntohs(pkt->seq)
#define RTP_PKT_TS(pkt)     ntohl(pkt->time)
#define RTP_PKT_SSRC(pkt)   ntohl(pkt->ssrc)
#define RTP_PKT_DATA(pkt)   (pkt->data  + pkt->cc)
#define RTP_PAYLOAD_SIZE(pkt, pkt_len)    ((pkt) ? pkt_len - ((pkt->data)-(uint8_t *)pkt) - pkt->cc - ((*(((uint8_t *)pkt)+pkt_len-1)) * pkt->pad) : 0)

#define RTPPT_ISDYNAMIC(pt)    (pt >= 96)

#define RTP_TRANSPORT_SPEC              10
#define RTP_TRANSPORT_SOCKTYPE          11
#define RTP_TRANSPORT_DELIVERY          20
#define RTP_TRANSPORT_SRCADDR           30
#define RTP_TRANSPORT_SRCADDRSTR        31
#define RTP_TRANSPORT_DSTADDR           40
#define RTP_TRANSPORT_DSTADDRSTR        41
#define RTP_TRANSPORT_LAYERS            50
#define RTP_TRANSPORT_MODE              60
#define RTP_TRANSPORT_APPEND            70
#define RTP_TRANSPORT_TTL               80
#define RTP_TRANSPORT_MCSRTP            90
#define RTP_TRANSPORT_MCSRTCP           91
#define RTP_TRANSPORT_MCSPORTS          92
#define RTP_TRANSPORT_CLIRTP            100
#define RTP_TRANSPORT_CLIRTCP           101
#define RTP_TRANSPORT_CLIPORTS          102
#define RTP_TRANSPORT_SRVRTP            110
#define RTP_TRANSPORT_SRVRTCP           111
#define RTP_TRANSPORT_SRVPORTS          112
#define RTP_TRANSPORT_ILVDRTP           120
#define RTP_TRANSPORT_ILVDRTCP          121
#define RTP_TRANSPORT_INTERLEAVED       122
#define RTP_TRANSPORT_STREAMRTP         130
#define RTP_TRANSPORT_STREAMRTCP        131
#define RTP_TRANSPORT_STREAMS           132
#define RTP_TRANSPORT_SSRC              140

#define RTP_TRANSPORT_NOTSET            -1
#define RTP_TRANSPORT_SET                0
#define RTP_TRANSPORT_ERR                1

typedef struct {
    char *spec;
    uint32_t ssrc;
    sock_type type;
    enum modes { play, record } mode;
    int append;
    int layers;
    int ttl;
    enum deliveries { unicast, multicast } delivery;
    nms_transport RTP;
    nms_transport RTCP;
} rtp_transport;

struct rtp_ssrc_stats {
    uint16_t max_seq;         //!< highest seq number seen 
    uint32_t cycles;          //!< shifted count of seq number cycles 
    uint32_t base_seq;        //!< base seq number 
    uint32_t bad_seq;         //!< last 'bad' seq number + 1 
    uint32_t probation;       //!< sequ. pkts till source is valid 
    uint32_t received;        //!< RTP pkts received 
    uint32_t expected_prior;  //!< pkt expected at last interval 
    uint32_t received_prior;  //!< pkt received al last interval 
    uint32_t transit;         //!< relative trans time for prev pkt 
    double jitter;          //!< extimated jitter 
    struct timeval lastrtp; //!< last RTP pkt reception time 
    struct timeval lastsr;  //!< last RTCP SR pkt reception time 
    uint32_t ntplastsr[2];    //!< last RTCP SR pkt NTP reception time 
    uint32_t firstts;         //!< first pkt timestamp
    uint32_t lastts;          //!< last pkt timestamp
    struct timeval firsttv; //!< first pkt timeval 
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
    struct timeval tp;      //!< the last time an RTCP pkt was transmitted
    struct timeval tn;      //!< the next scheduled transmission time of an RTCP pkt
    uint32_t pmembers;      //!< the estimated number of session members at time tm was last recomputed
    uint32_t members;       //!< the most currente estimate for the number of the session members
    uint32_t senders;       //!< the most currente estimate for the number of senders in the session
    double rtcp_bw;         //!< the target RTCP bandwidth
    uint8_t we_sent;        //!< flag that is true if the app has sent data since the second previous RTCP Report was transmitted
    double avg_rtcp_size;   //!< the average Compound RTCP pkt size, in octets, over all RTCP pkts sent and received by this partecipant
    uint8_t initial;        //!< the flag that is true if the app has not yet sent an RTCP pkt
};

#define SSRC_KNOWN      0
#define SSRC_NEW        1
#define SSRC_RTPNEW     2
#define SSRC_RTCPNEW    3
#define SSRC_COLLISION  4

typedef struct rtp_ssrc_s {
    uint32_t ssrc;
    nms_sockaddr rtp_from;
    nms_sockaddr rtcp_from;
    nms_sockaddr rtcp_to;
    int no_rtcp;
    struct rtp_ssrc_stats ssrc_stats;
    struct rtp_ssrc_descr ssrc_sdes;
    struct playout_buff_t * po;
    struct rtp_session_s *rtp_sess;     //!< RTP session SSRC belogns to.
    void *privs[128];                   //!< I would like to keep rtp able to manage dynamic payload changes at its best.
    struct rtp_ssrc_s *next;            //!< next known SSRC
    struct rtp_ssrc_s *next_active;     //!< next active SSRC
    int done_seek;
    void *park;                         //!< private pointer used by the application (e.g. to hold decoder state variables)
} rtp_ssrc;

struct rtp_conflict {
    nms_sockaddr transaddr;
    time_t time;
    struct rtp_conflict *next;
};

/** 
 * the <tt>rtp_parser_init</tt> function is called at rtp thread start
 * (in <tt>rtp_thread_create</tt>) for all the parsers registered for announced
 * payload types (present in the <tt>announced_fmts</tt> list)
 */
typedef int (*rtp_parser_init) (struct rtp_session_s * rtp_sess, unsigned pt);
typedef int (*rtp_parser) (rtp_ssrc * stm_src, rtp_frame * fr,
               rtp_buff * conf);
typedef int (*rtp_parser_uninit) (rtp_ssrc * stm_src, unsigned pt);

typedef struct rtp_session_s {
    void * owner; //!< rtsp_thread owning this rtp session
    uint32_t local_ssrc;
    rtp_transport transport;
    struct rtp_session_stats sess_stats;
    rtp_ssrc *ssrc_queue;                   //!< queue of all known SSRCs
    rtp_ssrc *active_ssrc_queue;            //!< queue of active SSRCs
    struct rtp_conflict *conf_queue;
    struct buffer_pool_t * bp;
    struct rtp_session_s *next;
    pthread_mutex_t syn;
    rtp_pt *ptdefs[128];                    //!< payload type definitions for the session (included dynamically defined)
    rtp_fmts_list *announced_fmts;          //!< list of rtp pt announced in sdp description (if present)
    // parsers functions
    rtp_parser_init parsers_inits[128];
    rtp_parser parsers[128];
    rtp_parser_uninit parsers_uninits[128];
    void *park;                             //!< private pointer used by the application (e.g. to hold decoder state variables)
} rtp_session;

typedef struct {
    int run;

    rtp_session *rtp_sess_head;

    // struct timeval startime;
    unsigned int prebuffer_size;

    pthread_mutex_t syn;
    pthread_t rtp_tid;
    pthread_t rtcp_tid;
} rtp_thread;

enum rtp_protos {
    RTP,
    RTCP
};

/**
 * rtp_fill error codes
 */
#define RTP_FILL_OK         0
#define RTP_BUFF_EMPTY      91
#define RTP_PARSE_ERROR     92
#define RTP_PKT_UNKNOWN     93
#define RTP_IN_PRM_ERR      94
#define RTP_SSRC_NOTVALID   97
#define RTP_BUFFERING       99

#define RTP_PKT_DATA_LEN(pkt, len) (len > 0) ? len - ((uint8_t *)(pkt->data)-(uint8_t *)pkt) - pkt->cc - ((*(((uint8_t *)pkt)+len-1)) * pkt->pad) : 0

/**
 * RTP Layer
 * @defgroup rtp_layer RTP Layer
 * @{
 */


/**
 * RTP Thread
 * @defgroup rtp_thread RTP Thread
 * @{
 */
rtp_thread *rtp_init(void);
int rtp_thread_create(rtp_thread *);    // something like rtp_run could be better?
/**
 * @}
 */


/**
 * RTP Payload Types
 * @defgroup rtp_payload_type RTP Payload Types
 * @{
 */
int rtp_announce_pt(rtp_session * rtp_sess, unsigned pt,
            rtp_media_type media_type);
int rtp_dynpt_reg(rtp_session * rtp_sess, unsigned pt, char *mime);

rtp_pt * rtp_get_pt_info(rtp_session * rtp_sess, unsigned pt);
/**
 * @}
 */


/**
 * RTP buffer handling
 * @defgroup rtp_recv RTP packet handling
 * @{
 */
int rtp_recv(rtp_session *);
/**
 * @}
 */


/**
 * RTP SSRC Queue
 * @defgroup rtp_ssrc_queue RTP SSRC Queue
 * @{
 */
rtp_ssrc *rtp_active_ssrc_queue(rtp_session * rtp_sess_head);
rtp_ssrc *rtp_next_active_ssrc(rtp_ssrc * ssrc);

int rtp_ssrc_check(rtp_session *, uint32_t, rtp_ssrc **, nms_sockaddr *,
           enum rtp_protos);
int rtp_ssrc_init(rtp_session *, rtp_ssrc **, uint32_t, nms_sockaddr *,
          enum rtp_protos);
/**
 * @}
 */


/**
 * RTP buffer handling
 * @defgroup rtp_buffer RTP Buffer Access
 * @{
 */
int rtp_fill_buffers(rtp_thread *);
int rtp_fill_buffer(rtp_ssrc *, rtp_frame *, rtp_buff *);

double rtp_get_next_ts(rtp_ssrc *);
int16_t rtp_get_next_pt(rtp_ssrc *);
int rtp_get_fps(rtp_ssrc *);

rtp_pkt *rtp_get_n_pkt(rtp_ssrc *, unsigned int *, uint32_t);
rtp_pkt *rtp_get_pkt(rtp_ssrc *, size_t *);
inline int rtp_rm_pkt(rtp_ssrc *);
void rtp_rm_all_pkts(rtp_ssrc *);
/**
 * @}
 */

/**
 * RTP transport setup functions
 * @defgroup rtp_transport RTP transport functions
 * @{
 */
int rtp_transport_set(rtp_session *, int, void *);
int rtp_transport_get(rtp_session *, int, void *, uint32_t);
/**
 * @}
 */

/**
 * RTP session functions
 * @defgroup rtp_session RTP session functions
 * @{
 */
rtp_session *rtp_session_init(nms_sockaddr *local, nms_sockaddr *peer);
struct rtsp_ctrl_t;
rtp_ssrc * rtp_session_get_ssrc(rtp_session *sess, struct rtsp_ctrl_t *ctl);
/**
 * @}
 */

/**
 * @{
 */

//************************************ internal functions

/**
 * RTP transport wrapper functions for rtp_transport_get
 */
inline char *rtp_get_spec(rtp_session *);
inline enum sock_types rtp_get_socktype(rtp_session *);
inline enum deliveries rtp_get_delivery(rtp_session *);
inline int rtp_get_srcaddrstr(rtp_session *, char *, uint32_t);
inline nms_addr *rtp_get_srcaddr(rtp_session *);
inline int rtp_get_dstaddrstr(rtp_session *, char *, uint32_t);
inline nms_addr *rtp_get_dstaddr(rtp_session *);
inline int rtp_get_layers(rtp_session *);
inline enum modes rtp_get_mode(rtp_session *);
inline int rtp_get_append(rtp_session *);
inline int rtp_get_ttl(rtp_session *);
inline int rtp_get_mcsports(rtp_session *, in_port_t[2]);
inline in_port_t rtp_get_mcsrtpport(rtp_session *);
inline in_port_t rtp_get_mcsrtcpport(rtp_session *);
inline int rtp_get_srvports(rtp_session *, in_port_t[2]);
inline in_port_t rtp_get_srvrtpport(rtp_session *);
inline in_port_t rtp_get_srvrtcpport(rtp_session *);
inline int rtp_get_cliports(rtp_session *, in_port_t[2]);
inline in_port_t rtp_get_clirtpport(rtp_session *);
inline in_port_t rtp_get_clirtcpport(rtp_session *);
inline int rtp_get_interleaved(rtp_session *, uint8_t[2]);
inline uint8_t rtp_get_ilvdrtp(rtp_session *);
inline uint8_t rtp_get_ilvdrtcp(rtp_session *);
inline int rtp_get_streams(rtp_session *, uint16_t[2]);
inline uint16_t rtp_get_rtpstream(rtp_session *);
inline uint16_t rtp_get_rtcpstream(rtp_session *);
inline uint32_t rtp_get_ssrc(rtp_session *);

// rtp transport wrapper functions for rtp_transport_set
// inline char *rtp_set_spec(rtp_session *, char *); // not settable
inline int rtp_set_delivery(rtp_session *, enum deliveries);
inline int rtp_set_socktype(rtp_session *, enum sock_types);
inline int rtp_set_srcaddrstr(rtp_session *, char *);
inline int rtp_set_srcaddr(rtp_session *, nms_addr *);
inline int rtp_set_dstaddrstr(rtp_session *, char *);
inline int rtp_set_dstaddr(rtp_session *, nms_addr *);
inline int rtp_set_layers(rtp_session *, int);
inline int rtp_set_mode(rtp_session *, enum modes);
inline int rtp_set_append(rtp_session *, int);
inline int rtp_set_ttl(rtp_session *, int);
inline int rtp_set_mcsports(rtp_session *, in_port_t[2]);
inline int rtp_set_mcsrtpport(rtp_session *, in_port_t);
inline int rtp_set_mcsrtcpport(rtp_session *, in_port_t);
inline int rtp_set_srvports(rtp_session *, in_port_t[2]);
inline int rtp_set_srvrtpport(rtp_session *, in_port_t);
inline int rtp_set_srvrtcpport(rtp_session *, in_port_t);
inline int rtp_set_cliports(rtp_session *, in_port_t[2]);
inline int rtp_set_clirtpport(rtp_session *, in_port_t);
inline int rtp_set_clirtcpport(rtp_session *, in_port_t);
inline int rtp_set_interleaved(rtp_session *, uint8_t[2]);
inline int rtp_set_ilvdrtp(rtp_session *, uint8_t);
inline int rtp_set_ilvdrtcp(rtp_session *, uint8_t);
inline int rtp_set_streams(rtp_session *, uint16_t[2]);
inline int rtp_set_rtpstream(rtp_session *, uint16_t);
inline int rtp_set_rtcpstream(rtp_session *, uint16_t);
inline int rtp_set_ssrc(rtp_session *, uint32_t);

// parsers
#define RTP_PRSR_ERROR      -1
#define RTP_DST_TOO_SMALL   -2
#define RTP_REG_STATIC      -3

void rtp_parsers_init(void);
int rtp_parser_reg(rtp_session *, int16_t, char *);
void rtp_parsers_new(rtp_parser * new_parsers,
             rtp_parser_init * new_parsers_inits);
inline void rtp_parser_set_uninit(rtp_session * rtp_sess, unsigned pt,
                  rtp_parser_uninit parser_uninit);

#endif /* NEMESI_RTP_H */
