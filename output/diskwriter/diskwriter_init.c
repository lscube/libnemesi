/* * 
 *  $Id$
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

#include <nemesi/diskwriter.h>
#include <nemesi/utils.h>
#include <fcntl.h>
#include <unistd.h>

NMSDiskWriter *diskwriter_init(const char *basename)
{
	NMSDiskWriter *dc;
	int fd;
	// struct disk_buff *disk_buffer;

	if ((dc=malloc(sizeof(NMSDiskWriter))) == NULL) {
		uierror("Could not alloc disk writer structure");
		return NULL;
	}

	if (!basename || !strlen(basename))
		dc->basename = strdup(DEFAULT_FILENAME);
	else
		dc->basename = strdup(basename);


	if ( (fd=open( dc->basename, O_EXCL)) > 0 ) {
		close(fd);
	} else if ( (fd=creat( dc->basename, 00644)) > 0 ) {
		close(fd);
		unlink( dc->basename );
	} else {
		fprintf(stderr, "\nError initializzing Disk Writer Module: you have not write permission\n");
		return NULL;
	}

	for(fd=0;fd<=MAX_PT;fd++)
		fd=-1;
	
	/*
	if ( (disk_buffer=db_init()) == NULL ) {
		fprintf(stderr, "\nFailed while initializing Disk Writer Buffer\n");
		return 1;
	}

	global_disk_buffer=disk_buffer;
	*/
	
	return dc;
}

