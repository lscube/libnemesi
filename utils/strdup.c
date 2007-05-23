/* * 
 *  $Id: dummy.h 48 2003-11-10 16:01:50Z mancho $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *      
 *      Giampaolo "mancho" Mancini - manchoz@inwind.it
 *    Francesco "shawill" Varano - shawill@infinto.it
 *
 *  NeMeSI is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  NeMeSI is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with NeMeSI; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *  
 * */

#include <config.h>

#if !HAVE_STRDUP
#include <nemesi/utils.h>

char *strdup(const char *s)
{
    char *dup;

    if ((dup = (char *) malloc(strlen(s) + 1)) == NULL) {
        perror("strdup");
        return NULL;
    }
    strcpy(dup, s);

    return dup;
}

// #else
// char *strdup(const char *s);
#endif                /* !HAVE_STRDUP */
