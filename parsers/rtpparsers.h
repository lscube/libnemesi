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

#ifndef RTPFRAMERS_H_
#define RTPFRAMERS_H_

#include "rtp.h"

/**
 * Depayloader info
 */
typedef struct {
    /// -1 terminated list of served static payload numbers (MUST be <96)
    int16_t static_pt;
    /// NULL terminated list of encoding names (usually the media subtype)"
    char *mime[];
} rtpparser_info;

/**
 * Depayloader class
 */
typedef struct {
    const rtpparser_info *served;     //!< Depayloader info
    rtp_parser_init init;       //!< Optional initialization
    rtp_parser parse;           //!< rtp parse/depayload function
    rtp_parser_uninit uninit;   //!< Optional deinitialization
} rtpparser;

// parsers
#define RTP_PRSR_ERROR      -1
#define RTP_DST_TOO_SMALL   -2
#define RTP_REG_STATIC      -3

void rtp_parsers_init(void);
int rtp_parser_check(char *);
int rtp_parser_reg(rtp_session *, int16_t, char *);
void rtp_parsers_new(rtp_parser * new_parsers,
             rtp_parser_init * new_parsers_inits,
             rtp_parser_uninit * new_parsers_uninits);

#endif                /* RTPFRAMERS_H_ */
