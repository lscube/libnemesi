/* * 
 *  $Id$
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

/* DOC++ */
/**
 * Esegue la connessione al "server" tramite la porta "port".
 * @param server stringa contenente il nome del server.
 * @param port porta alla quale agganciare il socket.
 * @param sock variabile che conterra' il descrittore del socket ottenuto.
 * @return 1 se la connessione e' avvenuta con successo, 0 altrimenti.
 * @see tcp_open
 */
int server_connect(char *host, char *port, int *sock, enum sock_types sock_type)
{
	int n;
	struct addrinfo *res, *ressave;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(struct addrinfo));

	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_UNSPEC;
	if (sock_type == TCP)
		hints.ai_socktype = SOCK_STREAM;
	else if (sock_type == UDP)
		hints.ai_socktype = SOCK_DGRAM;

	if ((n = gethostinfo(&res, host, port, &hints)) != 0) {
		uiprintf("%s: %s\n", PROG_NAME, gai_strerror(n));
		return 1;
	}

	ressave = res;

	do {
		if ((*sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol)) < 0)
			continue;

		if (connect(*sock, res->ai_addr, res->ai_addrlen) == 0)
			break;

		if (close(*sock) < 0) {
			uiprintf("%s: %s", PROG_NAME, strerror(errno));
			return 1;
		}

	} while ((res = res->ai_next) != NULL);

	if (res == NULL){
		uiprintf("Server connect error for \"%s:%s\"\n", host, port);
		return 1;
	}

	freeaddrinfo(ressave);

	return 0;
}
