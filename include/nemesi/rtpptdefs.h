/* * 
 *  ./include/nemesi/rtpptdefs.h: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:10 $
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

#ifndef __RTP_PT_DEFS_H
#define __RTP_PT_DEFS_H

#include <config.h>

#include <nemesi/types.h>

#define RTP_DEF_CLK_RATE 8000

#define RTP_DEF_MAX_NAME_LEN 16

typedef enum {
	AU = 0,
	VI = 1,
	AV = 2,
	NA = 3
} rtp_media_type_t;

struct rtp_pt_def {
	char name[RTP_DEF_MAX_NAME_LEN];	/* Encoding Name */
	rtp_media_type_t type;			/* Media Type: (A)udio, (V)ideo,  (A)udio(/)(V)ideo, (N)ot(/)(A)pplicable */
	uint32 rate; 				/* Clock Rate - in Hertz*/
	uint8 channels;				/* Channels */
};

extern struct rtp_pt_def rtp_pt_defs[];
extern int (*decoders[])(char *, int, uint8 *(*)());

#endif
