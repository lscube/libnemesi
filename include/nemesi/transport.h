#ifndef __NEMESI_TRANSPORT_H__
#define __NEMESI_TRANSPORT_H__

/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2007 by team@streaming.polito.it
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

#include <netembryo/wsocket.h>
#include <nemesi/types.h>

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

typedef struct {
    Sock sock;       //!< netembryo socket layer
    char *buffer;    /*!< for storing not completely transmitted data */
    union {
        struct {
            nms_addr srcaddr;    //!< stored in network order
            nms_addr dstaddr;     //!< stored in network order
        } udp;
        struct {
            uint8 ilvd;        //!< stored in host order
        } tcp;
        struct {
            uint16 stream;        //!< stored in host order
        } sctp;
    } u;

    /*human readable datas */
    in_port_t multicast_port;        //!< stored in host order
 /**/} nms_transport;

//---------------- Socket support functions ------------------ //
int sockaddr_get_nms_addr(const struct sockaddr *sockaddr, nms_addr * retaddr);
int sockaddr_cmp(struct sockaddr *addr1, socklen_t addr1_len, struct sockaddr *addr2, socklen_t addr2_len);

char *nms_addr_ntop(const nms_addr * addr, char *str, size_t len);
int nms_sockaddr_dup(nms_sockaddr * dst, nms_sockaddr * src);
int nms_addr_cmp(const nms_addr * addr1, const nms_addr * addr2);

// --------------- Transport Layer Wrapper API --------------- //
void nmst_init(nms_transport *);
int nmst_close(nms_transport *);
int nmst_read(nms_transport *, void *, size_t, void *);
int nmst_write(nms_transport *, void *, size_t, void *);
inline int nmst_is_active(nms_transport *);
// ----------- End of Transport Layer Wrapper API ----------- //

#endif
