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

#include <nemesi/rtp.h>

/*
 * Connect local client to oremote rtcp input port in order to send our rtcp statistics.
 *
 * \param stm_src the stream source whose statistics are.
 * \param remoteaddr sockaddr of remote address.
 * \param port remote rtcp port.
 * \return 0 on OK, 1 if connection went wrong, -1 on internal fatal error.
 * */
// int rtcp_to_connect(rtp_ssrc *stm_src, nms_sockaddr *remoteaddr, uint16 port)
int rtcp_to_connect(rtp_ssrc * stm_src, nms_addr * remoteaddr, in_port_t port)
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
