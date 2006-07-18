/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#ifndef RTPPTFRAMER_H_
#define RTPPTFRAMER_H_

#include "rtpparsers.h"

/*! the <tt>rtp_parser_init</tt> function is called at rtp thread start (in <tt>rtp_thread_create</tt>)
 * for all the parsers registered for announced payload types (present in the <tt>announced_fmts</tt> list)
 * */
//static int rtp_init_parser(struct _rtp_session *rtp_sess, unsigned pt);
static int rtp_parse(rtp_ssrc *, rtp_frame *, rtp_buff *);

/*! the parser could define an "unint function" of the type:
 * static int rtp_uninit_parser(rtp_ssrc *stm_src, unsigned pt);
 * and link this function to che corresponding pointer in 
 * <tt>rtp_parser_uninit *rtp_parsers_uninits</tt> array in rtp_session struct.
 * */
 
#define RTP_PARSER(x) rtpparser rtp_parser_##x = { \
	&served, \
	NULL, \
	rtp_parse, \
	NULL \
}

#define RTP_PARSER_FULL(x) \
	static int rtp_init_parser(struct _rtp_session *rtp_sess, unsigned pt); \
	static int rtp_parser_uninit(rtp_ssrc *stm_src, unsigned pt); \
	rtpparser rtp_parser_##x = {\
		&served, \
		rtp_init_parser, \
		rtp_parse, \
		rtp_uninit_parser \
	}

#endif /*RTPPTFRAMER_H_*/
