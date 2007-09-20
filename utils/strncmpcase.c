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

int strncmpcase(const char *s1, const char *s2, size_t n)
{
    char *str1;
    char *str2;
    int res;
    unsigned int i;

    if ((str1 = strdup(s1)) == NULL)
        return 1;

    if ((str2 = strdup(s2)) == NULL)
        return 1;

    for (i = 0; i < strlen(str1); i++)
        str1[i] = tolower(str1[i]);

    for (i = 0; i < strlen(str2); i++)
        str2[i] = tolower(str2[i]);

    res = strncmp(str1, str2, n);

    free(str1);
    free(str2);

    return res;
}
