/* * 
 *  ./include/nemesi/rtcp.h: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:10 $
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

#ifndef __RTCP_H
#define __RTCP_H

#include <config.h>

#include <unistd.h>

#include <nemesi/types.h>
#include <nemesi/comm.h>
#include <nemesi/rtp.h>
#include <nemesi/wsocket.h>


#define MAX_PKT_SIZE 548 /* 576 - 20 - 8 = Minimum Reassembly Buffer Size - IP datagram header -  UDP hedaer: in octects */
#define MAX_SDES_LEN 255 /* in octects */

#define RTCP_MIN_TIME 5.0
#define RTCP_SENDER_BW_FRACTION 0.25
#define RTCP_RCVR_BW_FRACTION 0.75
#define COMPENSATION 1.21828 /* e - 1.5 */

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
	RTPC_SDES_EMAIL = 3,
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

#define RTCP_VALID_MASK ( 0xc000 | 0x2000 | 0xfe ) /* ver | pad | pt */
#define RTCP_VALID_VALUE ( (RTP_VERSION << 14) | RTCP_SR )

#else

#define RTCP_VALID_MASK ( 0xfe00 | 0x20 | 0xc0 ) /* pad | ver | pt */
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

struct RTCP_Event {
	struct RTP_Session *rtp_sess;
	struct timeval tv;
	rtcp_type_t type;
	struct RTCP_Event *next;
};

typedef struct rtcp_sdes rtcp_sdes_t;

void *rtcp(void *);
int rtcp_thread_create(struct RTP_Session *rtp_sess);
void rtcp_clean(void *);

int rtcp_recv(struct RTP_Session *);
int rtcp_hdr_val_chk(rtcp_pkt *, int);

int parse_rtcp_pkt(struct Stream_Source *, rtcp_pkt *, int);
int parse_rtcp_sr(struct Stream_Source *, rtcp_pkt *);
int parse_rtcp_sdes(struct Stream_Source *, rtcp_pkt *);
int parse_rtcp_rr(rtcp_pkt *);
int parse_rtcp_bye(rtcp_pkt *);
int parse_rtcp_app(rtcp_pkt *);

int set_ssrc_sdes(struct Stream_Source *, rtcp_sdes_item_t *);

double rtcp_interval(int, int, double, int, double, int);

struct RTCP_Event *rtcp_schedule(struct RTCP_Event *, struct RTP_Session *, struct timeval, rtcp_type_t);
struct RTCP_Event *rtcp_deschedule(struct RTCP_Event *);

struct RTCP_Event *handle_rtcp_event(struct RTCP_Event *);
int send_rtcp_rr(struct RTP_Session *);
int build_rtcp_rr(struct RTP_Session *, rtcp_pkt *);
int build_rtcp_sdes(struct RTP_Session *, rtcp_pkt *, int);
int send_rtcp_bye(struct RTP_Session *);

#endif
