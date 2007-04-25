/* * 
 *  $Id:set_transport_str.c 267 2006-01-12 17:19:45Z shawill $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
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

#include <nemesi/rtsp.h>
#include <nemesi/utils.h>

int set_transport_str(rtp_session * rtp_sess, char **str)
{
    char buff[256];
    enum sock_types type;

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
#ifndef HAVE_SCTP_NEMESI
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
        sprintf(buff + strlen(buff), "ssrc=%u;",
            rtp_get_ssrc(rtp_sess));

    /* eliminiamo l'ultimo ; */
    /* drop last ';' */
    *(buff + strlen(buff) - 1) = '\0';

    if (!(*str = strdup(buff)))
        return nms_printf(NMSML_FATAL,
                  "set_transport_str: Could not duplicate string!\n");

    return 0;
}
