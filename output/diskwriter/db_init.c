/* * 
 *  ./output/diskwriter/db_init.c: $Revision: 1.2 $ -- $Date: 2003/01/22 16:17:36 $
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - manchoz@inwind.it
 *	Francesco "shawill" Varano - shawill@infinto.it
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

/* definizione per l'allocazione del buffer globale
 * questa definizione deve essere fatta solo in un file.
 *  */
#define GLOBAL_DISK_BUFFER

#include <nemesi/diskwriter.h>

struct disk_buff *db_init(void)
{
	struct disk_buff *buff;

	if ( (buff = (struct disk_buff *)malloc(sizeof(struct disk_buff))) == NULL ) {
		uiprintf("Cannot allocate memory.\n");
		return NULL;
	}

	buff->len = 0;
	buff->file_fd=-1;

	db_get(0, buff);

	return buff;
}

