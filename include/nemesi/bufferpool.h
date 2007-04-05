/* * 
 *  $Id:bufferpool.h 271 2006-01-20 18:45:51Z shawill $
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

/*! \file bufferpool.h 
 * \brief Header for \b bufferpool library.
 * */

/*! \defgroup bufferpool Bufferpool Library
 * 
 * \brief Memory handling.
 *
 * \b bufferpool consists in functions to manipulate statically allocated 
 * fixed size slots of memory.
 *
 * It also contains the high level functions to manage the playout buffer and
 * the RTP packets queue.
 *
 * This is used so that between receiving and decoding an RTP packet you can 
 * have just a single copy to memory.
 *
 * @{ */

#ifndef __BUFFERPOOL_H
#define __BUFFERPOOL_H
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <nemesi/types.h>

/*! The number of slots consisting the Playout Buffer. */			/* #define BP_SLOT_NUM 50 */
/* 1000ms / 20ms = Playout Buffer Size (in seconds) / Required RTP payload size (in seconds) */
#define BP_SLOT_NUM 150		// Bigger buffer. For video needs.

/*! Slot size.  Derived from <em>"Minimum Reassembly Buffer Size"</em> IPv4: the maximal dimension of non fragmented packets */

/*#define BP_SLOT_SIZE 548 *//* 576 - 20 - 8 = Minimum Reassembly Buffer Size - IP datagram header -  UDP hedaer */
#define BP_SLOT_SIZE 2048	/* bigger slot */

/*! Buffer Pool size */
#define BP_SIZE BP_SLOT_NUM*BP_SLOT_SIZE	/* 1 sec G.711 - 8kHz * 8bit
						   per sample */


/*! \brief Network Playout Buffer Slots.
 *
 * It represents the memory used to hold the RTP packet received.
 *
 * Never use it directly.
 * */
typedef struct {
	char x[BP_SLOT_SIZE];	/*!< Memory used to hold the packets 
                                     received from the network, 
                                     NEVER ACCESS IT DIRECTLY! */
} bp_slot;

/*!
* \brief Network Playout Element
*
* The elements are part of a indexed double linked list.
*
* \note the list delimiter is \c -1
* */
typedef struct {
	int pktlen; /*!< Lenght of the packet held */
	int prev;   /*!< Index of the previous element */
	int next;   /*!< index of the next element */
} poitem;

/*!
 * \brief Network Playout Buffer.
 *
 * \see poinit
 * \see poadd
 * \see podel
 * */
typedef struct {
	bp_slot **bufferpool;	    /*!< Pointer to memory space allocated for 
                                         Bufferpool and defined elsewhere. 
                                         \see bpinit */
	poitem pobuff[BP_SLOT_NUM]; /*!< Array that will keep the sorted
                                         list of slots containing arrived
                                         packets. */
	pthread_mutex_t po_mutex;   /*!< Mutex variable used for access control
                                         to shared structures of playout 
                                         buffer. */
	/* pthread_cond_t cond_empty; *//*!< Conditional variable used for 
                                            signaling in case of playout buffer
                                            empty */
	int pocount;		    /*!< List members count. */
	int pohead;		    /*!< List head. */
	int potail;		    /*!< List tail. */
	uint32 cycles;		    /*!< Counts how many times the RTP 
                                         \c timestamp rolls over */
                                         // shawill: probably to be moved.
} playout_buff;

/*!
* \brief Buffer Pool Management.
*
* \see bpinit
* \see bpkill
* \see bpget
* \see bpfree
* \see bprmv
* */
typedef struct {
	bp_slot *bufferpool;	        /*!< Pointer to the Bufferpool memory.
                                            \see bpinit */
	pthread_mutex_t fl_mutex;	/*!< Mutex to access the Bufferpool
                                             internals. */
	pthread_cond_t cond_full;	/*!< Signals if the Bufferpool is 
                                             full */
	int freelist[BP_SLOT_NUM];	/*!< Free slots. */
	int flhead;		        /*!< Free List head. */
	int flcount;		        /*!< Free List count. */
} buffer_pool;

#define PKT_DUPLICATED	1
#define PKT_MISORDERED	2

int poinit(playout_buff *, buffer_pool *);
int poadd(playout_buff *, int, uint32);
int podel(playout_buff *, int);
int bpinit(buffer_pool *);
int bpkill(buffer_pool *);
int bpget(buffer_pool *);
int bpfree(buffer_pool *, int);
int bprmv(buffer_pool *, playout_buff *, int);
#endif
/* @} */