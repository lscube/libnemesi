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

#ifndef RTPFRAMERS_H_
#define RTPFRAMERS_H_

#include <nemesi/rtp.h>

typedef struct {
    int16 static_pt;    // -1 terminated list of served static payload numbers (MUST be <96)
    char *mime[];        // NULL terminated list of served mime tipes in the form "type/subtype"
} rtpparser_info;

typedef struct {
    rtpparser_info *served;
    rtp_parser_init init;
    rtp_parser parse;
    rtp_parser_uninit uninit;
} rtpparser;

#endif                /* RTPFRAMERS_H_ */
