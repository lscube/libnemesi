/* *
 * This file is part of libnemesi
 *
 * Copyright (C) 2007 by LScube team <team@streaming.polito.it>
 * See AUTHORS for more details
 *
 * libnemesi is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * libnemesi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with libnemesi; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * */

#include "config.h"

#include <string.h>

#include "nemesi/transport.h"
#include "nemesi/comm.h"

#ifndef WIN32
#ifdef    HAVE_SOCKADDR_DL_STRUCT
# include    <net/if_dl.h>
#endif
#ifdef    AF_UNIX
#include <sys/un.h>
#endif
#endif

#ifdef ENABLE_SCTP
#include <netinet/sctp.h>
#endif

/**
 * Retrieves the address family and address itself from a sockaddr struct
 * @param sockaddr The sockaddr from which to retrieve the data
 * @param retaddr Where to save the retrieved data
 * @return 0 or 1 if socket family is unknown or sockaddr/retaddr pointers are NULL
 */
int sockaddr_get_nms_addr(const struct sockaddr *sockaddr, nms_addr * retaddr)
{
    if (!sockaddr || !retaddr)
        return 1;

    retaddr->family = sockaddr->sa_family;
    switch (sockaddr->sa_family) {
    case AF_INET:
        memcpy(&retaddr->addr.in,
               &((struct sockaddr_in *) sockaddr)->sin_addr,
               sizeof(struct in_addr));
        return 0;
        break;
#ifdef IPV6
    case AF_INET6:
        memcpy(&retaddr->addr.in6,
               &((struct sockaddr_in6 *) sockaddr)->sin6_addr,
               sizeof(struct in6_addr));
        return 0;
        break;
#endif
    default:
        retaddr->family = AF_UNSPEC;
        break;
    }

    return 1;
}

static int sock_cmp_addr(const struct sockaddr *sa1, const struct sockaddr *sa2)
{
    if (sa1->sa_family != sa2->sa_family)
        return -1;

    switch (sa1->sa_family) {
    case AF_INET:
        return (memcmp
            (&((struct sockaddr_in *) sa1)->sin_addr,
             &((struct sockaddr_in *) sa2)->sin_addr,
             sizeof(struct in_addr)));
        break;

#ifdef    IPV6
    case AF_INET6:
        return (memcmp
            (&((struct sockaddr_in6 *) sa1)->sin6_addr,
             &((struct sockaddr_in6 *) sa2)->sin6_addr,
             sizeof(struct in6_addr)));
        break;
#endif

#if !defined(WIN32) && defined(AF_UNIX)
    case AF_UNIX:
        return (strcmp
            (((struct sockaddr_un *) sa1)->sun_path,
             ((struct sockaddr_un *) sa2)->sun_path));
        break;
#endif

#ifdef    HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK:
        return -1;    /* no idea what to compare here ? */
        break;
#endif
    default:
        return -1;
        break;
    }
    // return -1;
}


/*
 * The function compares port filed of sock addr structure.
 * It recognizes the family of sockaddr struct and compares the rigth field.
 * \return 0 if port are equal, -1 if family is not known.
 * */
static int sock_cmp_port(const struct sockaddr *sa1, const struct sockaddr *sa2)
{
    if (sa1->sa_family != sa2->sa_family)
        return -1;

    switch (sa1->sa_family) {
    case AF_INET:
        return !(((struct sockaddr_in *) sa1)->sin_port ==
             ((struct sockaddr_in *) sa2)->sin_port);
        break;

#ifdef    IPV6
    case AF_INET6:
        return !(((struct sockaddr_in6 *) sa1)->sin6_port ==
             ((struct sockaddr_in6 *) sa2)->sin6_port);
        break;
#endif
    default:
        return -1;
        break;

    }
    // return -1;
}

/*
 * The function that compares two sockaddr structures.
 * \param addr1 first sockaddr struct
 * \param addr1_len length of first sockaddr struct
 * \param addr2 second sockaddr struct
 * \param addr1_len length of second sockaddr struct
 * \return 0 if the two structires are egual, otherwise an error reflecting the
 * first difference encountered.
 */
int sockaddr_cmp(struct sockaddr *addr1, socklen_t addr1_len, struct sockaddr *addr2, socklen_t addr2_len)
{
    if (addr1_len != addr2_len)
        return WSOCK_ERRSIZE;
    if (addr1->sa_family != addr1->sa_family)
        return WSOCK_ERRFAMILY;
    if (sock_cmp_addr(addr1, addr2 /*, addr1_len */ ))
        return WSOCK_ERRADDR;
    if (sock_cmp_port(addr1, addr2 /*, addr1_len */ ))
        return WSOCK_ERRPORT;

    return 0;
}

/**
 * Converts the address from nms_addr to human readable string
 */
char *nms_addr_ntop(const nms_addr * addr, char *str, size_t len)
{
    switch (addr->family) {
    case AF_INET:
        if (inet_ntop(AF_INET, &addr->addr.in, str, len) == NULL)
            return (NULL);
        return (str);
        break;
#ifdef    IPV6
    case AF_INET6:
        if (inet_ntop(AF_INET6, &addr->addr.in6, str, len) == NULL)
            return (NULL);
        return (str);
        break;
#endif

#if 0                // not yet supported by nms_addr
#ifdef    AF_UNIX
    case AF_UNIX:
        /* OK to have no pathname bound to the socket: happens on
         * every connect() unless client calls bind() first. */
        if (addr->addr.un_path[0] == 0)
            strcpy(str, "(no pathname bound)");
        else
            snprintf(str, len, "%s", addr->addr.un_path);
        return (str);
#endif

#ifdef    HAVE_SOCKADDR_DL_STRUCT
    case AF_LINK:
        if (addr->addr.dl_nlen > 0)
            snprintf(str, len, "%*s", addr->addr.dl_nlen,
                 &addr->addr.dl_data[0]);
        else
            snprintf(str, len, "AF_LINK, index=%d",
                 addr->addr.dl_index);
        return (str);
#endif
#endif
    default:
        snprintf(str, len, "addr_ntop: unknown AF_xxx: %d",
             addr->family);
        return (str);
    }
    return (NULL);
}

/**
 * Creates a copy of an nms_sockaddr structure
 */
int nms_sockaddr_dup(nms_sockaddr * dst, nms_sockaddr * src)
{

    if (!(dst->addr = malloc(src->addr_len)))
        return -nms_printf(NMSML_FATAL, "Cannot allocate memory\n");
    memcpy(dst->addr, src->addr, src->addr_len);
    dst->addr_len = src->addr_len;

    return 0;
}

/**
 * Compares two addresses in nms_addr format
 */
int nms_addr_cmp(const nms_addr * addr1, const nms_addr * addr2)
{
    if (addr1->family != addr2->family)
        return WSOCK_ERRFAMILY;
    switch (addr1->family) {
    case AF_INET:
        if (!memcmp
            (&addr1->addr.in, &addr2->addr.in, sizeof(struct in_addr)))
            return 0;
        else
            return WSOCK_ERRADDR;
        break;
    case AF_INET6:
        if (!memcmp
            (&addr1->addr.in6, &addr2->addr.in6,
             sizeof(struct in6_addr)))
            return 0;
        else
            return WSOCK_ERRADDR;
        break;
    default:
        return WSOCK_ERRFAMILYUNKNOWN;
    }

    return 0;
}


//Transport Layer Wrappers
int nmst_read(nms_transport * transport, void *buffer, size_t nbytes, void *protodata)
{
    switch (transport->sock.socktype) {
    case TCP:
        return recv(transport->sock.fd, buffer, nbytes, 0);
        break;
#ifdef ENABLE_SCTP
    case SCTP:
        if (!protodata) {
            return -1;
        }
        return sctp_recvmsg(transport->sock.fd, buffer, nbytes, NULL, 0,
                 (struct sctp_sndrcvinfo *) protodata, NULL);
        break;
#endif
    default:
        break;
    }
    return -1;
}

int nmst_write(nms_transport * transport, void *buffer, size_t nbytes, void *protodata)
{
#ifdef ENABLE_SCTP
    struct sctp_sndrcvinfo sinfo;
#endif
    switch (transport->sock.socktype) {
    case TCP:
        return send(transport->sock.fd, buffer, nbytes, 0);
        break;
#ifdef ENABLE_SCTP
    case SCTP:
        if (!protodata) {
            protodata = &sinfo;
            memset(protodata, 0, sizeof(struct sctp_sndrcvinfo));
        }
        return sctp_send(transport->sock.fd, buffer, nbytes,
            (struct sctp_sndrcvinfo *) protodata, MSG_EOR);
        break;
#endif
    default:
        break;
    }

    return -1;
}

inline int nmst_is_active(nms_transport * transport)
{
    return ((transport->sock.socktype != SOCK_NONE) && (transport->sock.fd >= 0));
}

void nmst_init(nms_transport * transport)
{
    memset(transport, 0, sizeof(nms_transport));

    // TCP is default protocol implemented for RTSP, so I init type to TCP.
    transport->sock.socktype = TCP;

    transport->sock.fd = -1;
}

int nmst_close(nms_transport * transport)
{
    if (transport->sock.remote_host)
        free(transport->sock.remote_host);

    // TODO should we do something else?
    return close(transport->sock.fd);
}


