/* * 
 * This file is part of NetEmbryo
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 * 
 * NetEmbryo is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * NetEmbryo is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with NetEmbryo; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *  
 * */

#ifndef NEMESI_TRANSPORT_H
#define NEMESI_TRANSPORT_H

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

#endif /* NEMESI_TRANSPORT_H */
