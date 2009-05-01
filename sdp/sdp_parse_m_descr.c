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

#include <string.h>
#include <stdlib.h>

#include "sdp.h"
#include "comm.h"

int sdp_parse_m_descr(sdp_medium_info * m_info, char *m_descr)
{
    char *tkn, *endtkn;

    if (!(tkn = strchr(m_descr, ' ')))
        return nms_printf(NMSML_ERR,
                  "SDP Media description string not valid: (m=%s)\n",
                  m_descr);
    *tkn = '\0';

    // parse media type
    if (!strcmp(m_descr, "video"))
        m_info->media_type = 'V';
    else if (!strcmp(m_descr, "audio"))
        m_info->media_type = 'A';
    else if (!strcmp(m_descr, "application"))
        m_info->media_type = 'P';
    else if (!strcmp(m_descr, "data"))
        m_info->media_type = 'D';
    else if (!strcmp(m_descr, "control"))
        m_info->media_type = 'C';

    *tkn = ' ';

    // parse port and number of ports
    m_info->port = strtol(tkn, &endtkn, 10);
    if (tkn == endtkn)
        return nms_printf(NMSML_ERR,
                  "SDP Media description string not valid: (m=%s)\n"
                  "Could not find port field\n",
                  m_descr);
    tkn = endtkn;        // + 1;
    if (*endtkn == '/') {
        m_info->n_ports = strtol(tkn + 1, &endtkn, 10);
        tkn = endtkn;    // + 1;
    } else
        m_info->n_ports = 1;

    for (; *tkn == ' '; tkn++);    // skip spaces
    if (!(*tkn))
        return nms_printf(NMSML_ERR,
                  "SDP Media description string not valid: (m=%s)\n"
                  "Could not find transport field\n",
                  m_descr);

    // parse transport protocol
    if (!(endtkn = strchr(tkn, ' ')))
        return nms_printf(NMSML_ERR,
                  "SDP Media description string not valid: (m=%s)\n"
                  "Description terminates whithout <fmt list>\n",
                  m_descr);
    *endtkn = '\0';
    strncpy(m_info->transport, tkn, 7);
    m_info->transport[7] = '\0';
    *endtkn = ' ';
    tkn = endtkn + 1;

    // fmt list: here we expect to store payload types
    for (; *tkn == ' '; tkn++);    // skip spaces
    m_info->fmts = tkn;

    return 0;
}
