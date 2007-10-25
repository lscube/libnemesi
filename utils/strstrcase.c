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

#include "utils.h"

/**
 * Case insensitive strstr
 */

char *strstrcase(char *haystack, const char *needle)
{
    char *ptr;
    unsigned len;

    if (haystack == NULL) return NULL;
    if (needle == NULL) return NULL;

    len = strlen(needle);
    if (len > strlen(haystack))
            return NULL;

    if (len == 0) return haystack;

    for (ptr = haystack; *(ptr + len - 1) != '\0'; ptr++)
        if (!strncasecmp (ptr, needle, len)) return ptr;

    return NULL;
}
