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

#include "rtspinternals.h"
#include "utils.h"

#if HAVE_LIBSCTP
static int get_transport_str_sctp(rtp_session * rtp_sess, char * tkna, char * tknb, char ** tokptr) {
    char str[256];
    uint16_t stream;
    do {
        if ((tkna = strstrcase(tknb, "server_streams"))) {
            for (; (*tkna == ' ') || (*tkna != '='); tkna++);
            for (tknb = tkna++; (*tknb == ' ') || (*tknb != '-');
                 tknb++);

            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            if ((stream = atoi(str)) >= MAX_SCTP_STREAMS) {
                nms_printf(NMSML_ERR,
                       "SCTP stream too high!\n");
                return 1;
            }
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMRTP,
                      &stream);

            for (tknb++; (*tknb == ' '); tknb++);

            for (tkna = tknb; (*tkna != '\0') && (*tkna != '\r')
                 && (*tkna != '\n'); tkna++);
            strncpy(str, tknb, sizeof(str));
            if (sizeof(str) <= tkna - tknb) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tkna - tknb] = '\0';
            }
            tkna++;
            if ((stream = atoi(str)) >= MAX_SCTP_STREAMS) {
                nms_printf(NMSML_ERR,
                       "SCTP stream too high!\n");
                return 1;
            }
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_STREAMRTCP,
                      &stream);

            continue;
        }
        if ((tkna = strstrcase(tknb, "ssrc"))) {
            uint32_t ssrc;

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            ssrc = strtoul(str, NULL, 16);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);

            continue;
        }
    } while ((tknb = strtok_r(NULL, ";", tokptr)));
    return 0;
}
#endif

static int get_transport_str_tcp(rtp_session * rtp_sess, char * tkna, char * tknb, char ** tokptr) {
    char str[256];
    int value;
    uint8_t ilvd;
    do {
        if ((tkna = strstrcase(tknb, "interleaved"))) {
            for (; (*tkna == ' ') || (*tkna != '='); tkna++);
            for (tknb = tkna++; (*tknb == ' ') || (*tknb != '-');
                 tknb++);

            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            if ((value = atoi(str)) > 255) {
                nms_printf(NMSML_ERR,
                       "Interleaved channel too high!\n");
                return 1;
            }
            ilvd = (uint8_t) value;
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTP,
                      &ilvd);

            for (tknb++; (*tknb == ' '); tknb++);

            for (tkna = tknb; (*tkna != '\0') && (*tkna != '\r')
                 && (*tkna != '\n'); tkna++);
            strncpy(str, tknb, sizeof(str));
            if (sizeof(str) <= tkna - tknb) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tkna - tknb] = '\0';
            }
            tkna++;
            if ((value = atoi(str)) > 255) {
                nms_printf(NMSML_ERR,
                       "Interleaved channel too high!\n");
                return 1;
            }
            ilvd = (uint8_t) value;
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_ILVDRTCP,
                      &ilvd);

            continue;
        }
        if ((tkna = strstrcase(tknb, "ssrc"))) {
            uint32_t ssrc;

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            ssrc = strtoul(str, NULL, 16);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);

            continue;
        }
    } while ((tknb = strtok_r(NULL, ";", tokptr)));
    return 0;
}

static int get_transport_str_udp(rtp_session * rtp_sess, char * tkna, char * tknb, char ** tokptr) {
    char str[256];
    in_port_t port;
    do {
        if ((tkna = strstrcase(tknb, "server_port"))
            || ((tkna = strstrcase(tknb, "port"))
            && !strncmp(tknb, "port", 4))) {

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);
            for (tknb = tkna++; (*tknb == ' ') || (*tknb != '-');
                 tknb++);

            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            port = atoi(str);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTP,
                      &port);

            for (tknb++; (*tknb == ' '); tknb++);

            for (tkna = tknb; (*tkna != '\0') && (*tkna != '\r')
                 && (*tkna != '\n'); tkna++);
            strncpy(str, tknb, sizeof(str));
            if (sizeof(str) <= tkna - tknb) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tkna - tknb] = '\0';
            }
            tkna++;
            port = atoi(str);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_SRVRTCP,
                      &port);

            continue;
        }
        if ((tkna = strstrcase(tknb, "source"))) {
            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            if (rtp_transport_set
                (rtp_sess, RTP_TRANSPORT_SRCADDRSTR, str)) {
                nms_printf(NMSML_ERR,
                       "Source IP Address not valid!\n");
                return 1;
            }
            continue;
        }
        if ((tkna = strstrcase(tknb, "destination"))) {
            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            if (rtp_transport_set
                (rtp_sess, RTP_TRANSPORT_DSTADDRSTR, str)) {
                nms_printf(NMSML_ERR,
                       "Destination IP Address not valid!\n");
                return 1;
            }
            continue;
        }
        if ((tkna = strstrcase(tknb, "ssrc"))) {
            uint32_t ssrc;

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            ssrc = strtoul(str, NULL, 16);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_SSRC, &ssrc);

            continue;
        }
        if ((tkna = strstrcase(tknb, "ttl"))) {
            int ttl;

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            ttl = atoi(str);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_TTL, &ttl);

            continue;
        }
        if ((tkna = strstrcase(tknb, "layers"))) {
            int layers;

            for (; (*tkna == ' ') || (*tkna != '='); tkna++);

            for (tknb = tkna++; (*tknb != '\0') && (*tknb != '\r')
                 && (*tknb != '\n'); tknb++);
            strncpy(str, tkna, sizeof(str));
            if (sizeof(str) <= tknb - tkna) {
                str[sizeof(str) - 1] = '\0';
            } else {
                str[tknb - tkna] = '\0';
            }
            tknb++;

            layers = atoi(str);
            rtp_transport_set(rtp_sess, RTP_TRANSPORT_LAYERS,
                      &layers);

            continue;
        }

    } while ((tknb = strtok_r(NULL, ";", tokptr)));
    return 0;
}

int get_transport_str(rtp_session * rtp_sess, char *buff)
{
    char str[256], *tokptr = NULL;
    char *tkna = str, *tknb = str;
    int n = 1;
    // char addr[128];              /* Unix domain is largest */

    memset(str, 0, sizeof(str));

    if (strstrcase(buff, RTP_AVP_TCP))
        rtp_sess->transport.type = TCP;
#ifdef HAVE_LIBSCTP
    else if (strstrcase(buff, RTP_AVP_SCTP))
        rtp_sess->transport.type = SCTP;
#endif
    else if (strstrcase(buff, RTP_AVP_UDP))
        rtp_sess->transport.type = UDP;
    else
        return n;

    for (tknb = strtok_r(buff, ";", &tokptr); (*tknb == ' ') || (*tknb == ':');
         tknb++);

    switch (rtp_sess->transport.type) {
    case UDP:
        n = get_transport_str_udp(rtp_sess, tkna, tknb, &tokptr);
        break;
    case TCP:
        n = get_transport_str_tcp(rtp_sess, tkna, tknb, &tokptr);
        break;
    case SCTP:
#ifdef HAVE_LIBSCTP
        n = get_transport_str_sctp(rtp_sess, tkna, tknb, &tokptr);
#endif
        break;
    default:
        break;
    }

    return n;
}


#if HAVE_LIBSCTP
static int set_transport_str_sctp(rtp_session * rtp_sess, char *buff)
{
    uint16_t streams[2];

    sprintf(buff + strlen(buff), "unicast;");

    if ( rtp_get_streams(rtp_sess, streams) == RTP_TRANSPORT_SET)
        sprintf(buff+strlen(buff), "streams=%u-%u;", streams[0], streams[1]);

    return 0;
}
#endif

static int set_transport_str_udp(rtp_session * rtp_sess, char *buff)
{
    //char addr[128];        /* Unix domain is largest */
    in_port_t ports[2];

    if (rtp_get_delivery(rtp_sess) == multicast)
        sprintf(buff + strlen(buff), "multicast;");
    else
        sprintf(buff + strlen(buff), "unicast;");
/*    if (rtp_transport_get
        (rtp_sess, RTP_TRANSPORT_DSTADDRSTR, addr,
         sizeof(addr)) == RTP_TRANSPORT_SET)
        sprintf(buff + strlen(buff), "destination=%s;", addr);
    if (rtp_transport_get
        (rtp_sess, RTP_TRANSPORT_SRCADDRSTR, addr,
         sizeof(addr)) == RTP_TRANSPORT_SET)
        sprintf(buff + strlen(buff), "source=%s;", addr); */
    if (rtp_get_layers(rtp_sess))
        sprintf(buff + strlen(buff), "layers=%d;",
            rtp_get_layers(rtp_sess));
    if (rtp_get_ttl(rtp_sess))
        sprintf(buff + strlen(buff), "ttl=%d;", rtp_get_ttl(rtp_sess));
    if (rtp_get_mcsports(rtp_sess, ports) == RTP_TRANSPORT_SET)
        sprintf(buff + strlen(buff), "port=%d-%d;", (int) ports[0],
            (int) ports[1]);
    if (rtp_get_cliports(rtp_sess, ports) == RTP_TRANSPORT_SET)
        sprintf(buff + strlen(buff), "client_port=%d-%d;",
            (int) ports[0], (int) ports[1]);

    return 0;
}

static int set_transport_str_tcp(rtp_session * rtp_sess, char *buff)
{
    uint8_t ilvds[2];

    sprintf(buff + strlen(buff), "unicast;");

    if ( rtp_get_interleaved(rtp_sess, ilvds) == RTP_TRANSPORT_SET)
        sprintf(buff+strlen(buff), "interleaved=%u-%u;", ilvds[0], ilvds[1]);

    return 0;
}

int set_transport_str(rtp_session * rtp_sess, char **str)
{
    char buff[256];
    sock_type type;

    memset(buff, 0, sizeof(buff));

    rtp_transport_get(rtp_sess, RTP_TRANSPORT_SPEC, buff, sizeof(buff));
    rtp_transport_get(rtp_sess, RTP_TRANSPORT_SOCKTYPE, &type, sizeof(type));

    switch (type) {
    case UDP:
        *(buff + strlen(buff)) = ';';
        set_transport_str_udp(rtp_sess, buff);
        break;
    case TCP:
        sprintf(buff+strlen(buff), "/TCP;");
        set_transport_str_tcp(rtp_sess, buff);
        break;
    case SCTP:
#ifndef HAVE_LIBSCTP
        return nms_printf(NMSML_FATAL,
            "set_transport_str: SCTP support not compiled in!\n");
#else
        sprintf(buff+strlen(buff), "/SCTP;");
        set_transport_str_sctp(rtp_sess, buff);
        break;
#endif
    default:
        return nms_printf(NMSML_FATAL,
            "set_transport_str: Unknown Transport type!\n");
    }

    if (rtp_get_mode(rtp_sess) == record)
        sprintf(buff + strlen(buff), "mode=record;");
    else
        sprintf(buff + strlen(buff), "mode=play;");
    if (rtp_get_append(rtp_sess))
        sprintf(buff + strlen(buff), "append;");
    if (rtp_get_ssrc(rtp_sess))
        sprintf(buff + strlen(buff), "ssrc=%08X;",
            rtp_get_ssrc(rtp_sess));

    /* eliminiamo l'ultimo ; */
    /* drop last ';' */
    *(buff + strlen(buff) - 1) = '\0';

    if (!(*str = strdup(buff)))
        return nms_printf(NMSML_FATAL,
                  "set_transport_str: Could not duplicate string!\n");

    return 0;
}

rtsp_medium *rtsp_med_create(rtsp_thread * t)
{
    int fd = t->transport.sock.fd;
    rtsp_medium *rtsp_m;
    struct sockaddr_storage localaddr, peeraddr;
    nms_sockaddr local =
        { (struct sockaddr *) &localaddr, sizeof(localaddr) };
    nms_sockaddr peer =
        { (struct sockaddr *) &peeraddr, sizeof(peeraddr) };

    getsockname(fd, (struct sockaddr *) local.addr, &local.addr_len);
    getpeername(fd, (struct sockaddr *) peer.addr, &peer.addr_len);

    if ((rtsp_m = (rtsp_medium *) calloc(1, sizeof(rtsp_medium))) == NULL) {
        nms_printf(NMSML_FATAL, "Cannot allocate memory.\n");
        return NULL;
    }

    if ((rtsp_m->rtp_sess = rtp_session_init(&local, &peer)) == NULL)
        return NULL;

    rtsp_m->rtp_sess->owner = t;
    return rtsp_m;
}
