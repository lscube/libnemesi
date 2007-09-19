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

/** @file rtp_ssrc_queue.c
 * This file contains the functions that manage the SSRC queue
 */

#include <nemesi/rtp.h>


/**
 * Gets the queue of active sources
 * @param rtp_sess_head The head of the rtp session queue. Can be retrieved from
                        the RTSP controller with rtsp_get_rtp_queue
 * @return The first active source, NULL if there isn't any.
 */
rtp_ssrc *rtp_active_ssrc_queue(rtp_session * rtp_sess_head)
{
    rtp_session *rtp_sess;

    for (rtp_sess = rtp_sess_head;
         rtp_sess && !rtp_sess->active_ssrc_queue;
         rtp_sess = rtp_sess->next);

    return rtp_sess ? rtp_sess->active_ssrc_queue : NULL;
}

/**
 * Gets the next active source
 * @param ssrc The source from which to retrieve the subsequent source
 * @return The next active source or NULL if ssrc was the last one
 */
rtp_ssrc *rtp_next_active_ssrc(rtp_ssrc * ssrc)
{
    rtp_session *rtp_sess;

    if (!ssrc)
        return NULL;

    if (ssrc->next_active)
        return ssrc->next_active;

    for (rtp_sess = ssrc->rtp_sess->next; rtp_sess;
         rtp_sess = rtp_sess->next)
        if (rtp_sess->active_ssrc_queue)
            return rtp_sess->active_ssrc_queue;

    return NULL;
}

/*
 * Connect local client to oremote rtcp input port in order to send our rtcp statistics.
 *
 * @param stm_src the stream source whose statistics are.
 * @param remoteaddr sockaddr of remote address.
 * @param port remote rtcp port.
 * @return 0 on OK, 1 if connection went wrong, -1 on internal fatal error.
 * */
static int rtcp_to_connect(rtp_ssrc * stm_src, nms_addr * remoteaddr, in_port_t port)
{
    char addr[128];        /* Unix domain is largest */
    char port_str[256];
    struct sockaddr_storage rtcp_to_addr_s;
    nms_sockaddr rtcp_to_addr =
        { (struct sockaddr *) &rtcp_to_addr_s, sizeof(rtcp_to_addr_s) };

    if (port > 0)
        // sprintf(port_str,"%d", ntohs(port));
        sprintf(port_str, "%d", port);
    else
        return nms_printf(NMSML_ERR,
                  "RTCP: Cannot connect to port (%d)\n", port);

    if (!nms_addr_ntop(remoteaddr, addr, sizeof(addr))) {
        nms_printf(NMSML_WARN,
               "RTP: Cannot get address from source\n");
        stm_src->no_rtcp = 1;
        return 1;
    } else
        nms_printf(NMSML_DBG2, "RTCP to host=%s\n", addr);

    /*if (server_connect(addr, port_str, &(stm_src->rtp_sess->transport.RTCP.fd), UDP)) {
        nms_printf(NMSML_WARN,
               "Cannot connect to remote RTCP destination %s:%s\n",
               addr, port_str);
        stm_src->no_rtcp = 1;
    }*/
    getsockname(stm_src->rtp_sess->transport.RTCP.sock.fd, rtcp_to_addr.addr,
            &rtcp_to_addr.addr_len);
    nms_sockaddr_dup(&stm_src->rtcp_to, &rtcp_to_addr);

    return 0;
}

/**
 * Initializes a new synchronization source and appens it to the queue
 * @param rtp_sess The RTP session for which to create the new source
 * @param stm_src A pointer to the pointer to initialize with the new source
 * @param ssrc The RTP ssrc value from which to create the source
 * @param recfrom The link from which to receive the data for the source
 * @param proto_type The protocol type for the source
 *
 * @return o if everything was ok, -1 on initialization errors
 */
int rtp_ssrc_init(rtp_session * rtp_sess, rtp_ssrc ** stm_src, uint32_t ssrc,
          nms_sockaddr * recfrom, enum rtp_protos proto_type)
{
    int addrcmp_err;
    nms_addr nms_address;

    if (((*stm_src) = (rtp_ssrc *) calloc(1, sizeof(rtp_ssrc))) == NULL)
        return -nms_printf(NMSML_FATAL, "Cannot allocate memory\n");

    (*stm_src)->next = rtp_sess->ssrc_queue;
    rtp_sess->ssrc_queue = *stm_src;

    (*stm_src)->ssrc = ssrc;
    (*stm_src)->no_rtcp = 0; //flag for connection errors
    (*stm_src)->rtp_sess = rtp_sess;

    if (proto_type == RTP) {
        nms_sockaddr_dup(&(*stm_src)->rtp_from, recfrom);
        nms_printf(NMSML_DBG2, "RTP/rtp_ssrc_init: proto RTP\n");
    } else if (proto_type == RTCP) {
        nms_sockaddr_dup(&(*stm_src)->rtcp_from, recfrom);
        nms_printf(NMSML_DBG2, "RTP/rtp_ssrc_init: proto RTCP\n");
    }

    if (rtp_sess->transport.type != UDP) {
        /*It is not needed to check addresses because data is received
        as interleaved or multistream from RTSP socket */    
        return 0;
    }

    if (sockaddr_get_nms_addr(recfrom->addr, &nms_address))
        return -nms_printf(NMSML_ERR,
                   "Address of received packet not valid\n");
    if (!
        (addrcmp_err =
         nms_addr_cmp(&nms_address, &rtp_sess->transport.RTP.u.udp.srcaddr))) {
        /* If the address from which we are receiving data is the
         * same to that announced in RTSP session, then we use RTSP
         * informations to set transport address for RTCP connection */
        if (rtcp_to_connect
            (*stm_src, &rtp_sess->transport.RTP.u.udp.srcaddr,
             (rtp_sess->transport).RTCP.sock.remote_port) < 0)
            return -1;
        nms_printf(NMSML_DBG2, "RTP/rtp_ssrc_init: from RTSP\n");

    } else if (proto_type == RTCP) {
        /* If we lack of informations we assume that net address of
         * RTCP destination is the same of RTP address and port is that
         * specified in RTSP*/
        if (rtcp_to_connect
            (*stm_src, &nms_address,
             (rtp_sess->transport).RTCP.sock.remote_port) < 0)
            return -1;
        nms_printf(NMSML_DBG2, "RTP/rtp_ssrc_init: from RTP\n");
    } else {
        switch (addrcmp_err) {
        case WSOCK_ERRFAMILY:
            nms_printf(NMSML_DBG2, "WSOCK_ERRFAMILY (%d!=%d)\n",
                   nms_address.family,
                   rtp_sess->transport.RTP.u.udp.srcaddr.family);
            break;
        case WSOCK_ERRADDR:
            nms_printf(NMSML_DBG2, "WSOCK_ERRADDR\n");
            break;
        case WSOCK_ERRFAMILYUNKNOWN:
            nms_printf(NMSML_DBG2, "WSOCK_ERRFAMILYUNKNOWN\n");
            break;
        }
        nms_printf(NMSML_DBG2,
               "RTP/rtp_ssrc_init: rtcp_to NOT set!!!\n");
        // return -1;
    }

    return 0;
}

/**
 * Checks the ssrc of incoming packets and creates a new synchronization source
 * if it wasn't already known.
 * @param rtp_sess The session for which to create the synchronization source
 * @param ssrc The RTP SSRC value
 * @param stm_ssrc Where to create the new synchronization source if it isn't already known
 * @param recfrom The link to check for the packets with the ssrc
 * @param proto_type The type of protocol used by the link
 *
 * @return SSRC_KNOWN, SSRC_NEW, SSRC_COLLISION, -1 on internal fatal error.
 * */
int rtp_ssrc_check(rtp_session * rtp_sess, uint32_t ssrc, rtp_ssrc ** stm_src,
           nms_sockaddr * recfrom, enum rtp_protos proto_type)
{
    struct rtp_conflict *stm_conf = rtp_sess->conf_queue;
    struct sockaddr_storage sockaddr;
    nms_sockaddr sock =
        { (struct sockaddr *) &sockaddr, sizeof(sockaddr) };
    int local_collision;


    local_collision = (rtp_sess->local_ssrc == ssrc) ? SSRC_COLLISION : 0;
    pthread_mutex_lock(&rtp_sess->syn);
    pthread_mutex_unlock(&rtp_sess->syn);
    for (*stm_src = rtp_sess->ssrc_queue;
         !local_collision && *stm_src && ((*stm_src)->ssrc != ssrc);
         *stm_src = (*stm_src)->next);
    if (!*stm_src && !local_collision) {
        /* new SSRC */
        pthread_mutex_lock(&rtp_sess->syn);
        nms_printf(NMSML_DBG1, "new SSRC\n");
        if (rtp_ssrc_init(rtp_sess, stm_src, ssrc, recfrom, proto_type)
            < 0) {
            pthread_mutex_unlock(&rtp_sess->syn);
            return -nms_printf(NMSML_ERR,
                       "Error while setting new Stream Source\n");
        }

        poinit(&((*stm_src)->po), &(rtp_sess->bp));
        pthread_mutex_unlock(&rtp_sess->syn);
        return SSRC_NEW;
    } else {
        if (local_collision) {

            if (proto_type == RTP)
                getsockname(rtp_sess->transport.RTP.sock.fd, sock.addr,
                        &sock.addr_len);
            else
                getsockname(rtp_sess->transport.RTCP.sock.fd, sock.addr,
                        &sock.addr_len);

        } else if (proto_type == RTP) {

            if (!(*stm_src)->rtp_from.addr) {
                nms_sockaddr_dup(&(*stm_src)->rtp_from, recfrom);
                nms_printf(NMSML_DBG1, "new SSRC for RTP\n");
                local_collision = SSRC_RTPNEW;
            }
            sock.addr = (*stm_src)->rtp_from.addr;
            sock.addr_len = (*stm_src)->rtp_from.addr_len;

        } else {    /* if (proto_type == RTCP) */


            if (!(*stm_src)->rtcp_from.addr) {
                nms_sockaddr_dup(&(*stm_src)->rtcp_from, recfrom);
                nms_printf(NMSML_DBG1, "new SSRC for RTCP\n");
                local_collision = SSRC_RTCPNEW;
            }
            sock.addr = (*stm_src)->rtcp_from.addr;
            sock.addr_len = (*stm_src)->rtcp_from.addr_len;

            if (rtp_sess->transport.type != UDP)
                return local_collision;

            if (!(*stm_src)->rtcp_to.addr) {
                nms_addr nms_address;

                if (sockaddr_get_nms_addr(recfrom->addr, &nms_address))
                    return -nms_printf(NMSML_ERR,
                               "Invalid address for received packet\n");

                // if ( rtcp_to_connect(*stm_src, recfrom, (rtp_sess->transport).RTCP.remote_port) < 0 )
                if (rtcp_to_connect
                    (*stm_src, &nms_address,
                     (rtp_sess->transport).RTCP.sock.remote_port) < 0)
                    return -1;
            }
        }

        if ((rtp_sess->transport.type == UDP) && sockaddr_cmp
            (sock.addr, sock.addr_len, recfrom->addr,
             recfrom->addr_len)) {
            nms_printf(NMSML_ERR,
                   "An identifier collision or a loop is indicated\n");

            /* An identifier collision or a loop is indicated */

            if (ssrc != rtp_sess->local_ssrc) {
                /* OPTIONAL error counter step not implemented */
                nms_printf(NMSML_VERB,
                       "Warning! An identifier collision or a loop is indicated.\n");
                return SSRC_COLLISION;
            }

            /* A collision or loop of partecipants's own packets */

            else {
                while (stm_conf
                       && sockaddr_cmp(stm_conf->transaddr.addr,
                              stm_conf->transaddr.
                              addr_len, recfrom->addr,
                              recfrom->addr_len))
                    stm_conf = stm_conf->next;

                if (stm_conf) {

                    /* OPTIONAL error counter step not implemented */

                    stm_conf->time = time(NULL);
                    return SSRC_COLLISION;
                } else {

                    /* New collision, change SSRC identifier */

                    nms_printf(NMSML_VERB,
                           "SSRC collision detected: getting new!\n");


                    /* Send RTCP BYE pkt */
                    /*       TODO        */

                    /* choosing new ssrc */
                    rtp_sess->local_ssrc = random32(0);
                    rtp_sess->transport.ssrc =
                        rtp_sess->local_ssrc;

                    /* New entry in SSRC queue with conflicting ssrc */
                    if ((stm_conf = (struct rtp_conflict *)
                         malloc(sizeof
                            (struct rtp_conflict))) ==
                        NULL)
                        return -nms_printf(NMSML_FATAL,
                                   "Cannot allocate memory!\n");

                    /* insert at the beginning of Stream Sources queue */
                    pthread_mutex_lock(&rtp_sess->syn);
                    if (rtp_ssrc_init
                        (rtp_sess, stm_src, ssrc, recfrom,
                         proto_type) < 0) {
                        pthread_mutex_unlock
                            (&rtp_sess->syn);
                        return -nms_printf(NMSML_ERR,
                                   "Error while setting new Stream Source\n");
                    }
                    poinit(&((*stm_src)->po),
                           &(rtp_sess->bp));
                    pthread_mutex_unlock(&rtp_sess->syn);

                    /* New entry in SSRC rtp_conflict queue */
                    nms_sockaddr_dup(&stm_conf->transaddr,
                            &sock);
                    stm_conf->time = time(NULL);
                    stm_conf->next = rtp_sess->conf_queue;
                    rtp_sess->conf_queue = stm_conf;
                }

            }
        }
    }

    return local_collision;
}
