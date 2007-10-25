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

/** @file rtcp_app.c
 * This file contains functions to parse application dependant packets
 */

#include "rtcp.h"
#include "comm.h"

/**
 * Parse application packet
 * @param pkt The packet itself
 * @return 0
 */
int rtcp_parse_app(rtcp_pkt * pkt)
{
    // TODO: handle app packet
    nms_printf(NMSML_DBG1, "Received APP from SSRC: %u\n", pkt->r.app.src);
    return 0;
}
