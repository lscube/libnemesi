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

#ifndef __SOCKET_H
#define __SOCKET_H

#include <config.h>

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>

#include <nemesi/comm.h>

#ifdef WORDS_BIGENDIAN
#define ntohl24(x) (x)
#else
#define ntohl24(x) (((x&0xff) << 16) | (x&0xff00) | ((x&0xff0000)>>16)) 
#endif

enum sock_types {
	TCP = 0,
	UDP = 1
};

int gethostinfo(struct addrinfo **, char *, char *, struct addrinfo *);
int tcp_open(struct sockaddr *, int);
int server_connect(char *, char *, int *, enum sock_types);
int server_create(char *, char *, int *);
int tcp_write(int, void *, int);
int tcp_read(int, void *, int);

#endif
