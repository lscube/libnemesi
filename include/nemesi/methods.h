/* * 
 *  ./include/nemesi/methods.h: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
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

#ifndef __METHODS_H
#define __METHODS_H

#include <config.h>

#define SETUP_TKN	"SETUP"
#define REDIRECT_TKN	"REDIRECT"
#define PLAY_TKN	"PLAY"
#define PAUSE_TKN	"PAUSE"
#define SESSION_TKN	"SESSION"
#define RECORD_TKN	"RECORD"
#define EXT_METHOD_TKN	"EXT-"
#define HTTP_GET	"GET"	/*aggiunto per supportare il get http */

/*  methods that have changed between drafts of the RTSP document */

/* #ifdef DRAFT02 */

#define HELLO_TKN	"OPTIONS"
#define GET_TKN		"DESCRIBE"
#define GET_PARAM_TKN	"GET_PARAMETER"
#define SET_PARAM_TKN	"SET_PARAMETER"
#define CLOSE_TKN	"TEARDOWN"

/* #else DRAFT01 (roughly; compatible with RealMedia beta 1) 
#define HELLO_TKN	"HELLO"
#define GET_TKN		"GET"
#define GET_PARAM_TKN	"GET_PARAM"
#define SET_PARAM_TKN	"SET_PARAM"
#define CLOSE_TKN	"STOP" 
#endif DRAFT02 | DRAFT01 
 */


/* method codes */
#define RTSP_SETUP_METHOD	0
#define RTSP_GET_METHOD		1
#define RTSP_REDIRECT_METHOD	2
#define RTSP_PLAY_METHOD	3
#define RTSP_PAUSE_METHOD	4
#define RTSP_SESSION_METHOD	5
#define RTSP_HELLO_METHOD	6
#define RTSP_RECORD_METHOD	7
#define RTSP_CLOSE_METHOD	8
#define RTSP_GET_PARAM_METHOD	9
#define RTSP_SET_PARAM_METHOD	10
#define RTSP_EXTENSION_METHOD	11
#define HTTP_GET_METHOD		12	/*aggiunto per il GET HTTP */

/*
 * method response codes.  These are 100 greater than their
 * associated method values.  This allows for simplified
 * creation of event codes that get used in event_handler()
 */
#define RTSP_SETUP_RESPONSE	100
#define RTSP_GET_RESPONSE	101
#define RTSP_REDIRECT_RESPONSE	102
#define RTSP_PLAY_RESPONSE	103
#define RTSP_PAUSE_RESPONSE	104
#define RTSP_SESSION_RESPONSE	105
#define RTSP_HELLO_RESPONSE	106
#define RTSP_RECORD_RESPONSE	107
#define RTSP_CLOSE_RESPONSE	108
#define RTSP_GET_PARAM_RESPONSE	109
#define RTSP_SET_PARAM_RESPONSE	110
#define RTSP_EXTENSION_RESPONSE	111

#define DESCRIPTION_SDP_FORMAT	1
#define	DESCRIPTION_MH_FORMAT	2

#endif
