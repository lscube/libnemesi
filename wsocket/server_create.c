/* * 
 *  ./wsocket/server_create.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:49 $
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

#include <nemesi/wsocket.h>
#include <nemesi/version.h>

int server_create(char *host, char *port, int *sock)
{
	int n;
	struct addrinfo *res, *ressave;
	struct addrinfo hints;
	
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ((n = gethostinfo(&res, host, port, &hints)) != 0) {
		uiprintf("%s: %s\n", PROG_NAME, gai_strerror(n));
		return 1;
	}

	ressave = res;

	do {
		if ((*sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;

		if (bind(*sock, res->ai_addr, res->ai_addrlen) == 0)
			break;

		if (close(*sock) < 0) {
			uiprintf("%s: %s\n", PROG_NAME, strerror(errno));
			return 1;
		}


	} while ((res = res->ai_next) != NULL);

	if (res == NULL)
		return 1;

	freeaddrinfo(ressave);

	return 0;
}
