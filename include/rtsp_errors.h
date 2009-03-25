/* * 
 * * This file is part of NetEmbryo
 *
 * Copyright (C) 2009 by LScube team <team@lscube.org>
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

#ifndef _NETEMBRYO_RTSP_ERRORS_H_
#define _NETEMBRYO_RTSP_ERRORS_H_

#define RTSP_MAX_REPLY_MESSAGE_LEN 256

/** 
 * RTSP Error Notification data and functions
 * @defgroup rtsp_error RTSP Error Notification
 * @{
 */

/**
  * RTSP reply message
  */
typedef struct
{
    int reply_code; //!< RTSP code representation of the message
    char reply_str[RTSP_MAX_REPLY_MESSAGE_LEN]; //!< written representation of the message
} RTSP_ReplyMessage;

/**
  * RTSP error description
  */
typedef struct
{
    RTSP_ReplyMessage message; //!< RTSP standard error message
    int got_error; //!< can be: FALSE no error, TRUE generic error or have internal error id
} RTSP_Error;


extern RTSP_Error const RTSP_Continue;
extern RTSP_Error const RTSP_Ok;
extern RTSP_Error const RTSP_Created;
extern RTSP_Error const RTSP_Accepted;
extern RTSP_Error const RTSP_BadRequest;
extern RTSP_Error const RTSP_Forbidden;
extern RTSP_Error const RTSP_NotFound;
extern RTSP_Error const RTSP_NotAcceptable;
extern RTSP_Error const RTSP_UnsupportedMedia;
extern RTSP_Error const RTSP_ParameterNotUnderstood;
extern RTSP_Error const RTSP_NotEnoughBandwidth;
extern RTSP_Error const RTSP_SessionNotFound;
extern RTSP_Error const RTSP_HeaderFieldNotValidforResource;
extern RTSP_Error const RTSP_InvalidRange;
extern RTSP_Error const RTSP_UnsupportedTransport;
extern RTSP_Error const RTSP_InternalServerError;
extern RTSP_Error const RTSP_NotImplemented;
extern RTSP_Error const RTSP_ServiceUnavailable;
extern RTSP_Error const RTSP_VersionNotSupported;
extern RTSP_Error const RTSP_OptionNotSupported;

void set_RTSP_Error(RTSP_Error * err, int reply_code, const char * message);
RTSP_Error const * get_RTSP_Error(int reply_code);
/**
 * @}
 */

#endif

