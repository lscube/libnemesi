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

#ifndef __RTCP_H
#define __RTCP_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>

#include <nemesi/types.h>
#include <nemesi/comm.h>
#include <nemesi/rtp.h>
#include <nemesi/transport.h>


#define MAX_PKT_SIZE 548    /* 576 - 20 - 8 = Minimum Reassembly Buffer Size - IP datagram header -  UDP hedaer: in octects */
#define MAX_SDES_LEN 255    /* in octects */

#define RTCP_MIN_TIME 5.0
#define RTCP_SENDER_BW_FRACTION 0.25
#define RTCP_RCVR_BW_FRACTION 0.75
#define COMPENSATION 1.21828    /* e - 1.5 */

typedef enum {
    RTCP_SR = 200,
    RTCP_RR = 201,
    RTCP_SDES = 202,
    RTCP_BYE = 203,
    RTCP_APP = 204
} rtcp_type_t;

typedef enum {
    RTCP_SDES_END = 0,
    RTCP_SDES_CNAME = 1,
    RTCP_SDES_NAME = 2,
    RTCP_SDES_EMAIL = 3,
    RTCP_SDES_PHONE = 4,
    RTCP_SDES_LOC = 5,
    RTCP_SDES_TOOL = 6,
    RTCP_SDES_NOTE = 7,
    RTCP_SDES_PRIV = 8
} rtcp_sdes_type_t;

typedef struct {
#ifdef WORDS_BIGENDIAN
    uint32 ver:2;
    uint32 pad:1;
    uint32 count:5;
#else
    uint32 count:5;
    uint32 pad:1;
    uint32 ver:2;
#endif
    uint32 pt:8;
    uint32 len:16;

} rtcp_common_t;

#ifdef WORDS_BIGENDIAN

#define RTCP_VALID_MASK ( 0xc000 | 0x2000 | 0xfe )    /* ver | pad | pt */
#define RTCP_VALID_VALUE ( (RTP_VERSION << 14) | RTCP_SR )

#else

#define RTCP_VALID_MASK ( 0xfe00 | 0x20 | 0xc0 )    /* pad | ver | pt */
#define RTCP_VALID_VALUE ( ( RTCP_SR << 8 ) | (RTP_VERSION << 6) )

#endif

typedef struct {
    uint32 ssrc;
    uint32 fraction:8;
    int32 lost:24;
    uint32 last_seq;
    uint32 jitter;
    uint32 last_sr;
    uint32 dlsr;
} rtcp_rr_t;

typedef struct {
    uint32 ntp_seq;
    uint32 ntp_frac;
    uint32 ntp_ts;
    uint32 psent;
    uint32 osent;
} rtcp_si_t;

typedef struct {
    uint8 type;
    uint8 len;
    uint8 data[1];
} rtcp_sdes_item_t;

typedef struct {
    rtcp_common_t common;
    union {
        struct {
            uint32 ssrc;
            rtcp_si_t si;
            rtcp_rr_t rr[1];
        } sr;

        struct {
            uint32 ssrc;
            rtcp_rr_t rr[1];
        } rr;

        struct rtcp_sdes {
            uint32 src;
            rtcp_sdes_item_t item[1];
        } sdes;

        struct {
            uint32 src[1];
        } bye;

        struct {
            uint32 src;
            char name[4];
            uint8 data[1];
        } app;
    } r;
} rtcp_pkt;

struct rtcp_event {
    rtp_session *rtp_sess;
    struct timeval tv;
    rtcp_type_t type;
    struct rtcp_event *next;
};

typedef struct rtcp_sdes rtcp_sdes_t;

void *rtcp(void *);
int rtcp_thread_create(rtp_thread *);
void rtcp_clean(void *);
void rtcp_clean_events(void *);

int rtcp_recv(rtp_session *);
int rtcp_hdr_val_chk(rtcp_pkt *, int);

int rtcp_parse_pkt(rtp_ssrc *, rtcp_pkt *, int);
int rtcp_parse_sr(rtp_ssrc *, rtcp_pkt *);
int rtcp_parse_sdes(rtp_ssrc *, rtcp_pkt *);
int rtcp_parse_rr(rtcp_pkt *);
int rtcp_parse_bye(rtcp_pkt *);
int rtcp_parse_app(rtcp_pkt *);

int rtcp_set_ssrc_sdes(rtp_ssrc *, rtcp_sdes_item_t *);

double rtcp_interval(int, int, double, int, double, int);

struct rtcp_event *rtcp_schedule(struct rtcp_event *, rtp_session *,
                 struct timeval, rtcp_type_t);
struct rtcp_event *rtcp_deschedule(struct rtcp_event *);

struct rtcp_event *rtcp_handle_event(struct rtcp_event *);
int rtcp_send_rr(rtp_session *);
int rtcp_build_rr(rtp_session *, rtcp_pkt *);
int rtcp_build_sdes(rtp_session *, rtcp_pkt *, int);
int rtcp_send_bye(rtp_session *);

#endif
