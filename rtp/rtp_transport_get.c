/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
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

#include <errno.h>

#include <nemesi/rtp.h>
#include <nemesi/utils.h>

int rtp_transport_get(struct RTP_Session *rtp_sess, int par, void *value, uint32 len)
{
	// switch here for parameters that do NOT need value
	if (!value)
		return 1;
	// switch here for parameters that need value
	switch (par) {
		case RTP_TRANSPORT_SRCADDR:
			memcpy((NMSaddr *)value, &rtp_sess->transport.srcaddr, min(sizeof(NMSaddr), len));
			if (len < sizeof(NMSaddr))
				errno = ENOSPC;
			return 0;
			break;
		case RTP_TRANSPORT_SRCADDRSTR:
			if ( addr_ntop(&rtp_sess->transport.srcaddr, (char *)value, len) )
				return 1;
			return 0;
			break;
		case RTP_TRANSPORT_DSTADDR:
			memcpy((NMSaddr *)value, &rtp_sess->transport.dstaddr, min(sizeof(NMSaddr), len));
			if (len < sizeof(NMSaddr))
				errno = ENOSPC;
			return 0;
			break;
		case RTP_TRANSPORT_DSTADDRSTR:
			if ( addr_ntop(&rtp_sess->transport.dstaddr, (char *)value, len) )
				return 1;
			return 0;
			break;
		default:
			return 1;
			break;
	}

	return 1;
}

