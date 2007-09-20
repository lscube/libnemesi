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

#include <nemesi/utils.h>

char *strstrcase(char *haystack, const char *needle)
{
    char *str1, *str2;
    char *ret;
    unsigned int i;

    if ((str1 = strdup(haystack)) == NULL)
        return NULL;

    if ((str2 = strdup(needle)) == NULL)
        return NULL;

    for (i = 0; i < strlen(str1); i++)
        str1[i] = tolower(str1[i]);

    for (i = 0; i < strlen(str2); i++)
        str2[i] = tolower(str2[i]);

    if ((ret = strstr(str1, str2)) != NULL)
        ret = haystack + (ret - str1);

    free(str1);
    free(str2);

    return ret;

}
