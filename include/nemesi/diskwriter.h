/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#include <nemesi/types.h>
#include <nemesi/cc.h>

#define DEFAULT_FILENAME "nemesi.out"
#define MAX_PT 127

typedef struct {
	char *basename;
	cc_tag *tag[MAX_PT+1];
	char *ext[MAX_PT+1];
	int fd[MAX_PT+1];
} nms_diskwriter;

nms_diskwriter *diskwriter_init(const char *);
int get_file_fd(nms_diskwriter *, int);
int diskwriter(nms_diskwriter *, int, char *, int);
void close_files(int []);
int diskwriter_close(nms_diskwriter *);

#endif /* __DISKWRITER_H */

