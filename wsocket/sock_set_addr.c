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
 *  this piece of code is taken from Richard Stevens source code of
 *    UNIX Network Programming, Volume 1, Second Edition: Networking APIs:
 *      Sockets and XTI, Prentice Hall, 1998, ISBN 0-13-490012-X.
 * */

#include <nemesi/wsocket.h>

void sock_set_addr(struct sockaddr *sa, /* socklen_t salen,*/ const void *addr)
{
	switch (sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in	*sin = (struct sockaddr_in *) sa;
	
			memcpy(&sin->sin_addr, addr, sizeof(struct in_addr));
			return;
		}

#ifdef	IPV6
		case AF_INET6: {
			struct sockaddr_in6	*sin6 = (struct sockaddr_in6 *) sa;
	
			memcpy(&sin6->sin6_addr, addr, sizeof(struct in6_addr));
			return;
		}
#endif
	}

	return;
}

