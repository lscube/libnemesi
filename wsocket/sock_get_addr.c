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

#include <nemesi/wsocket.h>

int sock_get_addr(const struct sockaddr *sockaddr, NMSaddr *retaddr)
{
	if ( !sockaddr || !retaddr)
		return 1;

	retaddr->family = sockaddr->sa_family;
	switch (sockaddr->sa_family) {
		case AF_INET:
			memcpy(&retaddr->addr.in, &((struct sockaddr_in *)sockaddr)->sin_addr, sizeof(struct in_addr));
			return 0;
			break;
#ifdef IPV6
		case AF_INET6:
			memcpy(&retaddr->addr.in6, &((struct sockaddr_in6 *)sockaddr)->sin6_addr, sizeof(struct in6_addr));
			return 0;
			break;
#endif
		default:
			retaddr->family = AF_UNSPEC;
			break;
	}

	return 1;
}

