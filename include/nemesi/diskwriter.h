/* * 
 *  ./include/nemesi/diskwriter.h: $Revision: 1.5 $ -- $Date: 2003/07/08 15:58:01 $
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

#ifndef __DISKWRITER_H
#define __DISKWRITER_H

#include <stdio.h>
#include <stdarg.h>

#include <nemesi/types.h>

#ifndef GLOBAL_DISK_BUFFER
#define DISK_EXTERN extern
#else
#define DISK_EXTERN
#endif

#define DEFAULT_FILENAME "nemesi.out"

#define FREQ 44100
#define CHANNELS 2
#define BYTES_X_SAMPLE 2

/* Disk Buffer defines */
#define SECONDS 1
#define DISK_BUFF_SIZE (unsigned long)(FREQ*SECONDS*CHANNELS*BYTES_X_SAMPLE)

struct disk_buff {
	uint8 data[DISK_BUFF_SIZE];
	uint32 len;
	int file_fd;
};

DISK_EXTERN struct disk_buff *global_disk_buffer;

struct disk_buff *db_init(void);
uint8 *db_get(uint32, ...);
/* end of Disk Buffer defines */

int diskwriter_init(void);
int get_file_fd(void);
int diskwriter(char *data, int len);
void close_file(void);
int diskwriter_close(void);

#undef DISK_EXTERN

#endif /* __DISKWRITER_H */

