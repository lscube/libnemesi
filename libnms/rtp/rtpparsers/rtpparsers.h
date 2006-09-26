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

#ifndef RTPFRAMERS_H_
#define RTPFRAMERS_H_

#include <nemesi/rtp.h>

typedef struct {
	int16 static_pt;	// -1 terminated list of served static payload numbers (MUST be <96)
	char *mime[];		// NULL terminated list of served mime tipes in the form "type/subtype"
} rtpparser_info;

typedef struct {
	rtpparser_info *served;
	rtp_parser_init init;
	rtp_parser parse;
	rtp_parser_uninit uninit;
} rtpparser;

#endif				/* RTPFRAMERS_H_ */
