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

#include <stdlib.h>

#include <nemesi/sdp.h>
#include <nemesi/comm.h>

int sdp_set_attr(sdp_attr ** attr_list, char *a)
{
    sdp_attr *new, **i;

    // we use calloc, so it's all already initialized to NULL
    if (!(new = (sdp_attr *) calloc(1, sizeof(sdp_attr))))
        return nms_printf(NMSML_FATAL, "Could not allocate memory\n");

    new->name = a;
    new->value = strstr(a,":");
    *new->value++ = '\0';

    for (i = attr_list; *i; i = &((*i)->next)); // search for the tail of queue
    *i = new;

    return 0;
}
