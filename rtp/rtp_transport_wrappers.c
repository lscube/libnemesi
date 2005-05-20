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

#include <nemesi/rtp.h>

inline char *rtp_transport_get_spec(struct RTP_Session *rtp_sess)
{
	return rtp_sess->transport.spec;
}

inline enum deliveries rtp_transport_get_delivery(struct RTP_Session *rtp_sess)
{
	return rtp_sess->transport.delivery;
}

inline int rtp_trasnport_get_srcaddrstr(struct RTP_Session *rtp_sess, char *addrstr, uint32 strlen)
{
	return rtp_transport_get(rtp_sess, RTP_TRANSPORT_DSTADDRSTR, addrstr, strlen);
}

