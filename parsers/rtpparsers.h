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

#include <nemesi/rtp.h>

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
    rtpparser_info *served;     //!< Depayloader info
    rtp_parser_init init;       //!< Optional initialization
    rtp_parser parse;           //!< rtp parse/depayload function
    rtp_parser_uninit uninit;   //!< Optional deinitialization
} rtpparser;

#endif                /* RTPFRAMERS_H_ */
