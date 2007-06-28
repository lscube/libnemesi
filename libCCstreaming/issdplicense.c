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

#include <nemesi/cc.h>
#include <nemesi/utils.h>

#include <nemesi/comm.h>

int issdplicense(char *sdp_a)
{
    char *cclicenses[][2] = CC_LICENSE;
    unsigned int i;

    // shawill: sizeof(cclicenses)/sizeof(*cclicenses) == number of couples name-description present
    for (i = 0; i < sizeof(cclicenses) / sizeof(*cclicenses); i++) {
        if (!strncmpcase
            (sdp_a, cclicenses[i][CC_ATTR_NAME],
             strlen(cclicenses[i][CC_ATTR_NAME]))) {
            nms_printf(NMSML_DBG1,
                   "found valid cc field in SDP description (%s - %s)\n",
                   cclicenses[i][CC_ATTR_NAME],
                   cclicenses[i][CC_ATTR_DESCR]);
            return 1;
        }
    }

    return 0;
}
