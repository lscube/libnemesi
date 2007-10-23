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

#ifndef RTPPTFRAMER_H_
#define RTPPTFRAMER_H_

#include "rtpparsers.h"

/*! the parser could define an "unint function" of the type:
 * static int rtp_uninit_parser(rtp_ssrc *stm_src, unsigned pt);
 * and link this function to che corresponding pointer in 
 * <tt>rtp_parser_uninit *rtp_parsers_uninits</tt> array in rtp_session struct.
 * */

#define RTP_PARSER(x) rtpparser rtp_parser_##x = { \
    &x##_served, \
    NULL, \
    x##_parse, \
    NULL \
}

/**
 * the <tt>rtp_parser_init</tt> function is called at rtp thread start
 * (in <tt>rtp_thread_create</tt>)
 * for all the parsers registered for announced payload types
 * (present in the <tt>announced_fmts</tt> list)
 * */

#define RTP_PARSER_FULL(x) \
    rtpparser rtp_parser_##x = {\
        &x##_served, \
        x##_init_parser, \
        x##_parse, \
        x##_uninit_parser \
    }

#endif                /*RTPPTFRAMER_H_ */
