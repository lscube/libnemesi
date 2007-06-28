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

#include <nemesi/rtcp.h>

int rtcp_set_ssrc_sdes(rtp_ssrc * stm_src, rtcp_sdes_item_t * item)
{
    char *str = ((char **) (&(stm_src->ssrc_sdes)))[item->type];

    if (str != NULL) {
        if (memcmp(str, item->data, item->len) != 0) {
            free(str);
            if ((str =
                 (((char **) (&(stm_src->ssrc_sdes)))[item->
                                  type]) =
                 (char *) malloc(item->len + 1)) == NULL)
                return nms_printf(NMSML_FATAL,
                          "Cannot allocate memory!\n");

            memcpy(str, item->data, item->len);
            str[item->len] = 0;
        }

    } else {
        if ((str = ((char **) (&(stm_src->ssrc_sdes)))[item->type] =
             (char *) malloc(item->len + 1)) == NULL)
            return nms_printf(NMSML_FATAL,
                      "Cannot allocate memory!\n");

        memcpy(str, item->data, item->len);
        str[item->len] = 0;
    }
    return 0;
}
