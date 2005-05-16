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

int addrcmp(const NMSaddr *addr1, const NMSaddr *addr2)
{
	if ( addr1->family != addr2->family )
		return WSOCK_ERRFAMILY;
	switch (addr1->family) {
		case AF_INET:
			if (!memcmp(&addr1->addr.in, &addr2->addr.in, sizeof(struct in_addr)))
				return 0;
			else
				return WSOCK_ERRADDR;
			break;
		case AF_INET6:
			if (!memcmp(&addr1->addr.in6, &addr2->addr.in6, sizeof(struct in6_addr)))
				return 0;
			else
				return WSOCK_ERRADDR;
			break;
		default:
			return WSOCK_ERRFAMILYUNKNOWN;
	}
	
	return 0;
}

