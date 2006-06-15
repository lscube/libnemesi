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

#ifndef __DECODER_H
#define __DECODER_H

#define ENABLE_DECODER_2

#include <config.h>

#include <sys/time.h>
#include <pthread.h>

#include <nemesi/rtsp.h>
#include <nemesi/rtp.h>
#include <nemesi/comm.h>
#include <nemesi/bufferpool.h>
#include <nemesi/output.h>
#include <nemesi/types.h>

extern int (*decoders[128])(char *, int, nms_output *); // defined in decoder.c

void *decoder(void *);

//int dec_create(struct rtp_thread *);
pthread_t dec_create(struct rtsp_ctrl *);
void dec_idle(void);
int dec_destroy(pthread_t);
void dec_clean(void *);

#endif
