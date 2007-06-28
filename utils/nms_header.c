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

#include <string.h>

#include <nemesi/version.h>
#include <nemesi/comm.h>

inline void nms_header(void)
{
    char svnrev[32] = "";
#ifdef SVNREV
    char *tkn;

    // strcat(svnrev, SVNREV+1);
    snprintf(svnrev, sizeof(svnrev), "- SVN %s", SVNREV + 1);
    if ((tkn = strchr(svnrev, '$')))
        *tkn = '\0';
#endif
    nms_printf(NMSML_ALWAYS,
           "\n" NMSCLR_BLUE_BOLD "%s - %s -- release %s %s(%s)\n\n"
           NMSCLR_DEFAULT, PROG_NAME, PROG_DESCR, VERSION, svnrev,
           VERSION_NAME);
}
