/* * 
 *  ./include/nemesi/etui.h: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
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

#ifndef __E_TUI_H
#define __E_TUI_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <nemesi/version.h>
#include <nemesi/main.h>
#include <nemesi/utils.h>
#include <nemesi/comm.h>

#define CL_OPTIONS "hv"		/* help, version */

int ui(struct RTSP_args *, int, char **);
int parse_cl(int, char **, char **);
int parse_prompt(struct RTSP_args *, char *);
int seturlname(struct RTSP_args *, char *);
int throbber(struct RTSP_Thread *);
int send_open(struct RTSP_args *, char *);
int send_play(struct RTSP_args *, char *);
int send_pause(struct RTSP_args *, char);
int send_close(struct RTSP_args *);
int get_infos(struct RTSP_args *);

#endif
