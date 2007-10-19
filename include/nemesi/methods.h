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

#ifndef NEMESI_METHODS_H
#define NEMESI_METHODS_H
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
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


/* method codes */
#define RTSP_SETUP_METHOD        0
#define RTSP_GET_METHOD          1
#define RTSP_REDIRECT_METHOD     2
#define RTSP_PLAY_METHOD         3
#define RTSP_PAUSE_METHOD        4
#define RTSP_SESSION_METHOD      5
#define NEMESI_RTSP_HELLO_METHOD 6
#define RTSP_RECORD_METHOD       7
#define RTSP_CLOSE_METHOD        8
#define RTSP_GET_PARAM_METHOD    9
#define RTSP_SET_PARAM_METHOD    10
#define RTSP_EXTENSION_METHOD    11
#define HTTP_GET_METHOD          12    /* GET HTTP... */

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

/**
 * Status Code Definitions
 */
#define RTSP_SUCCESS                200
#define RTSP_REDIRECT               300
#define RTSP_MULTIPLE_CHOICES       300
#define RTSP_MOVED_PERMANENTLY      301 //!< Resource moved permanently
#define RTSP_FOUND                  302 //!< Resource reside temporarily (load balancing)
#define RTSP_SEE_OTHER              303 //!< This status code SHALL NOT be used in RTSP
#define RTSP_NOT_MODIFIED           304 //!< If the client has performed a conditional DESCRIBE or SETUP
#define RTSP_USE_PROXY              305
#define RTSP_CLIENT_ERROR           400
#define RTSP_BAD_REQUEST            400 //!< The request could not be understood by the server (bad syntax)
#define RTSP_METHOD_NOT_ALLOWED     405 //!< The method specified in the request is not allowed
#define RTSP_PAR_NOT_UNDERSTOOD     451 //!< The recipient of the request does not support one or more parameters
#define RTSP_RESERVED               452 //!< This error code was removed from RFC 2326 [1] and is obsolete
#define RTSP_NOT_ENOUGH_BAND        453 //!< The request was refused because there was insufficient bandwidth
#define RTSP_SESSION_NOT_FOUND      454 //!< The RTSP session identifier in the Session header is invalid
#define RTSP_METHOD_NOT_STATE       455 //!< The client or server cannot process this request in its current state
#define RTSP_FIELD_NOT_4_RES        456 //!< The server could not act on a required request header
#define RTSP_INVALID_RANGE          457 //!< The Range value given is out of bounds
#define RTSP_READONLY_PARAMETER     458 //!< The parameter to be set by SET_PARAMETER can be read but not modified
#define RTSP_NOT_AGGREGATE          459 //!< The method may not be applied on the URL since it is aggregate ctrl
#define RTSP_ONLY_AGGREGATE         460 //!< The method may not be applied on the URL since it is not aggregate ctrl
#define RTSP_UNSUPP_TRANSPORT       461 //!< The Transport field did not contain a supported transport specification
#define RTSP_DEST_UNREACHABLE       462 //!< The client address could not be reached.
#define RTSP_SERVER_ERROR           500
#define RTSP_UNSUPPORTED_OPTION     551 //!< Feature-tag given was not supported

#define RTSP_IS_SUCCESS(x) ((x>=RTSP_SUCCESS) && (x<RTSP_REDIRECT))? 1 : 0
#define RTSP_IS_REDIRECT(x) ((x>=RTSP_REDIRECT) && (x<RTSP_CLIENT_ERROR))? 1 : 0
#define RTSP_IS_CLIENT_ERROR(x) ((x>=RTSP_CLIENT_ERROR) && (x<RTSP_SERVER_ERROR))? 1 : 0
#define RTSP_IS_SERVER_ERROR(x) (x>=RTSP_SERVER_ERROR)? 1 : 0

#endif /* NEMESI_METHODS_H */
