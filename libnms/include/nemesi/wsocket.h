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
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <nemesi/comm.h>
#include <nemesi/types.h>

#ifdef WORDS_BIGENDIAN
#define ntohl24(x) (x)
#else
#define ntohl24(x) (((x&0xff) << 16) | (x&0xff00) | ((x&0xff0000)>>16)) 
#endif

#ifndef HAVE_STRUCT_SOCKADDR_STORAGE
#define MAXSOCKADDR 128 /*!< max socket address structure size */
struct sockaddr_storage {
	char padding[MAXSOCKADDR];
};
#endif // HAVE_STRUCT_SOCKADDR_STORAGE

typedef struct {
	struct sockaddr *addr;
	socklen_t addr_len;
} nms_sockaddr;

typedef struct {
	sa_family_t family;
	union {
		struct in_addr in;
		struct in6_addr in6;
	} addr;
} nms_addr;

enum sock_types {
	TCP = 0,
	UDP = 1
};

#define WSOCK_ERRFAMILYUNKNOWN	-1
#define WSOCK_ERRSIZE	1
#define WSOCK_ERRFAMILY	2
#define WSOCK_ERRADDR	3
#define WSOCK_ERRPORT	4

int gethostinfo(struct addrinfo **, char *, char *, struct addrinfo *);
int tcp_open(struct sockaddr *, int);
int server_connect(char *, char *, int *, enum sock_types);
int server_create(char *, char *, int *);
int tcp_write(int, void *, int);
int tcp_read(int, void *, int);
int sock_cmp_addr(const struct sockaddr *, const struct sockaddr * /*, socklen_t */);
int sock_cmp_port(const struct sockaddr *, const struct sockaddr * /*, socklen_t */);
void sock_set_port(struct sockaddr *, /* socklen_t,*/ int);
void sock_set_addr(struct sockaddr *, /* socklen_t,*/ const void *addr);
uint16 sock_get_port(const struct sockaddr * /*, socklen_t*/);
int sock_get_addr(const struct sockaddr *, nms_addr *);
char *sock_ntop_host(const struct sockaddr *, socklen_t, char *, size_t);
int sockaddrcmp(struct sockaddr *, socklen_t, struct sockaddr *, socklen_t);
int sockaddrdup(nms_sockaddr *, nms_sockaddr *);
int addrcmp(const nms_addr *, const nms_addr *);
char *addr_ntop(const nms_addr *, char *, size_t);

#endif
