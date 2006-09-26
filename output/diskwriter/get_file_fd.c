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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <nemesi/diskwriter.h>
#include <nemesi/comm.h>

int get_file_fd(nms_diskwriter * dc, int pt)
{
	char filename[256];
	uint32 written;

	if (pt > MAX_PT)
		return -nms_printf(NMSML_ERR, "Payload type exeded max allowed\n");

	if (dc->fd[pt] < 0) {
		cc_getag(pt, &(dc->tag[pt]), &(dc->ext[pt]));
		if (dc->ext[pt])
			sprintf(filename, "%s.%d.%s", dc->basename, pt, dc->ext[pt]);
		else
			sprintf(filename, "%s.%d", dc->basename, pt);
		if ((dc->fd[pt] = creat(filename, 00644)) < 0)
			nms_printf(NMSML_ERR, "file %s in current directory cannot be created\n", filename);
		if (dc->tag[pt]->hdim)
			if ((written = write(dc->fd[pt], dc->tag[pt]->header, dc->tag[pt]->hdim)) < dc->tag[pt]->hdim)
				nms_printf(NMSML_DBG1, "WARNING: only %d bytes of %d written\n", written,
					   dc->tag[pt]->hdim);
	}

	return dc->fd[pt];

	/*
	   if ( (dc->fd[pt] < 0) && ( (dc->fd[pt]=creat( DEFAULT_FILENAME, 00644 )) < 0 ) )
	   nms_printf(NMSML_ERR, "file %s in current directory cannot be created\n", DEFAULT_FILENAME);

	   if ( (global_disk_buffer->file_fd < 0) && ( (global_disk_buffer->file_fd=creat( DEFAULT_FILENAME, 00644 )) < 0 ) )
	   nms_printf(NMSML_ERR, "file %s in current directory cannot be created\n", DEFAULT_FILENAME);

	   return global_disk_buffer->file_fd;
	 */
}
