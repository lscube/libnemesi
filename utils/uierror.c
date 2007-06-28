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

#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#include <nemesi/comm.h>

#ifdef USE_UIPRINTF

/*!
  return 1, error
 */
int uierror(const char *fmt, ...)
{
    va_list ap;
    int fd;
    FILE *uistderr;

    if ((fd = dup(UIERROR_FILENO)) < 0) {
        fprintf(stderr, "\nfailed duplicating UIERROR_FILENO\n");
        return -1;
    }
    if (!(uistderr = fdopen(fd, "a"))) {
        fprintf(stderr, "\nfailed opening uistderr stream.\n");
        return -1;
    }

    va_start(ap, fmt);

    fprintf(uistderr, "error: ");
    vfprintf(uistderr, fmt, ap);
    fprintf(uistderr, "\n");

    va_end(ap);
    fclose(uistderr);
    close(fd);

    return 1;
}
#endif                // USE_UIPRINTF
