/* * 
 *  ./include/nemesi/bufferpool.h: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:10 $
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

#ifndef __BUFFERPOOL_H
#define __BUFFERPOOL_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <nemesi/types.h>

/** La lunghezza del Buffer di Playout. */
#define BP_SLOT_NUM 50		/* 1000ms / 20ms = Playout Buffer Size (in seconds) / Required RTP payload size (in seconds) */
/** La dimensione del singolo slot del Buffer Pool. */
#define BP_SLOT_SIZE 548	/* 576 - 20 - 8 = Minimum Reassembly Buffer Size - IP datagram header -  UDP hedaer */
/** La dimensione totale del Buffer Pool. */
#define BP_SIZE BP_SLOT_NUM*BP_SLOT_SIZE	/* 1 sec G.711 - 8kHz * 8bit per sample */


/* Da non usare. MAI. */
typedef struct {
	char x[BP_SLOT_SIZE];
} BP_SLOT;

/**
* @memo poitem
* @doc La struttura che rappresenta un elemento della lista double-linked del Buffer di Playout.
* */
typedef struct {
	int pktlen;
	int prev;
	int next;
} poitem;

typedef struct {
	BP_SLOT **bufferpool;
	poitem pobuff[BP_SLOT_NUM];
	int pohead;
	int potail;
	uint32 cycles;
} playout_buff;

/**
* @memo buffer_pool
* @doc La struttura per la gestione  del Buffer Pool.
* */
typedef struct {
	BP_SLOT *bufferpool;
	pthread_mutex_t fl_mutex;
	int freelist[BP_SLOT_NUM];
	int flhead;
	int flcount;
} buffer_pool;

int poinit(playout_buff *, buffer_pool *);
int poadd(playout_buff *, int, uint32);
int podel(playout_buff *, int);
int bpinit(buffer_pool *);
int bpkill(buffer_pool *);
int bpget(buffer_pool *);
int bpfree(buffer_pool *, int);
int bprmv(buffer_pool *, playout_buff *, int);
#endif
