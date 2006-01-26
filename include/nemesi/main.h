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

#ifndef __MAIN_H
#define __MAIN_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <nemesi/version.h>
#include <nemesi/rtsp.h>
#include <nemesi/output.h>
#include <nemesi/types.h>

typedef struct {
	char *audio;
	char *video;
	char *diskwriter;
	int32 sysbuff_ms;
} nms_out_hints;

typedef struct {
	uint8 gui;
	char *url;
} nms_ui_hints;

typedef struct {
	nms_out_hints *output;
	nms_ui_hints *ui;
	nms_rtsp_hints *rtsp;
} nms_cl_opts;

int output_init(nms_out_hints *);
int output_uninit(void);
int load_plugins(void);
void unload_plugins(void);

// commands to be used from ui
// for some of these we need only the rtsp function so it will be just a macro.
int nms_open(struct rtsp_ctrl *, char *, void (*)(void *), void *);
#define nms_play rtsp_play
#define nms_pause(x) rtsp_pause(x)
#define nms_stop(x) rtsp_stop(x)
#define nms_close rtsp_close // it will be replaced with function if decoder thread needs to be killed

#endif
