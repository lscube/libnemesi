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

#include <stdlib.h>

#include "sdp.h"
#include "comm.h"

int sdp_set_attr(sdp_attr ** attr_list, char *a)
{
    sdp_attr *new, **i;

    // we use calloc, so it's all already initialized to NULL
    if (!(new = (sdp_attr *) calloc(1, sizeof(sdp_attr))))
        return nms_printf(NMSML_FATAL, "Could not allocate memory\n");

    new->name = a;
    new->value = strstr(a,":");
    if (!new->value) {
        free(new);
        return 1;
    }
    *new->value++ = '\0';

    for (i = attr_list; *i; i = &((*i)->next)); // search for the tail of queue
    *i = new;

    return 0;
}

sdp_attr * sdp_get_attr(sdp_attr * attr_list, char * name)
{
    sdp_attr * cur_attr = NULL;

    for (cur_attr = attr_list; cur_attr; cur_attr = cur_attr->next) {
        if (!strcmp(cur_attr->name, name))
            break;
    }

    return cur_attr;
}

/**
 * Parses an SDP range attribute value
 * (currently only NPT format is supported, the code is based on
 *  feng's parse_play_time_range)
 */
sdp_range sdp_parse_range(char * value)
{
    sdp_range r = {0,0};
    char tmp[5] = {0, };

    if (!(value = strchr(value, '=')))
        return r;

    if (sscanf(value + 1, "%f", &r.begin) != 1) {
        r.begin = 0;
        if (sscanf(value + 1, "%4s", tmp) != 1 && !strcasecmp(tmp,"now-")) {
            return r;
        }
    }

    if (!(value = strchr(value, '-')))
        return r;

    if (sscanf(value + 1, "%f", &r.end) != 1)
        r.end = 0;

    return r;
}
