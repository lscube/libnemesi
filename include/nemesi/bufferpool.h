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

/*! \file bufferpool.h 
 * \brief Header contenente le definizioni della libreria \b bufferpool.
 * */

/*! \defgroup bufferpool Libreria Bufferpool
 * 
 * \brief Gestione ottimizzata della memoria.
 *
 * La liberia \b bufferpool mette a disposizione un buffer di memoria, allocato
 * staticamente e formato da slot di dimensione fissa, e le funzioni per la sua
 * gestione, tramite le quali il programma può richiedere e rilasciare degli
 * slot.  
 *
 * All'interno sono contenute, inoltre, le funzioni per la gestione del buffer
 * di playout di rete. Si tratta di una serie di strutture e primitive, poste
 * idealmente ad un livello superiore rispetto al bufferpool, che mantengono
 * una lista di pacchetti RTP, ordinati secondo il numero di sequenza, che
 * permettono di gestirne il riordino e la perdita.
 *
 * Una tale architettura è utilizzata in maniera efficiente da NeMeSI che può
 * ricevere un pacchetto RTP in uno slot del bufferpool, di cui si era fatta
 * richiesta in precedenza, dal quale non sarà più spostato fino alla sua
 * rimozione dopo il suo processamento da parte del decoder.  In seguito alla
 * ricezione dalla rete, il pacchetto sarà ordinato tramite le sovrastrutture
 * di indicizzazione che implementano il buffer di playout di rete.
 *
 * In altre parole: dalla ricezione alla decodifica c'è una sola copia in
 * memoria.
 *
 * @{ */

#ifndef __BUFFERPOOL_H
#define __BUFFERPOOL_H

#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <nemesi/types.h>

/*! La lunghezza, in slot, del Buffer di Playout. */
/* #define BP_SLOT_NUM 50 */	/* 1000ms / 20ms = Playout Buffer Size (in
				   seconds) / Required RTP payload size (in
				   seconds) */
#define BP_SLOT_NUM 150		// Bigger buffer. For video needs.

/*! La dimensione del singolo slot del Buffer Pool.  Il valore deriva dal
 * <em>"Minimum Reassembly Buffer Size"</em> di IPv4: questa, infatti, dovrebbe
 * essere la dimensione massima dei pacchetti RTP per essere certi che non
 * vengano frammentati dai router*/
/*#define BP_SLOT_SIZE 548*/	/* 576 - 20 - 8 = Minimum Reassembly Buffer
				   Size - IP datagram header -  UDP hedaer */

#define BP_SLOT_SIZE 2048	/* bigger slot */

/*! La dimensione totale, in byte, del Buffer Pool. */
#define BP_SIZE BP_SLOT_NUM*BP_SLOT_SIZE	/* 1 sec G.711 - 8kHz * 8bit
						   per sample */


/*! \brief Slot del Buffer di Playout di Rete.
 *
 * Questa struttura rappresenta la definizione di uno slot della zona di
 * memoria allocata per contenere i dati del Buffer di Playout di Rete.  Ognuno
 * di essi conterrà i pacchetti RTP arrivati dalla rete.
 *
 * Tale struttura non sarà mai usata direttamente se non nella fase di
 * allocazione della memoria destinata al buffer.
 * */
/* Da non usare. MAI. */
typedef struct {
	char x[BP_SLOT_SIZE]; /*!< Zona di memoria che immagazzinerà i
				pacchetti RTP provenienti dalla rete. MAI
				UTILIZZATA DIRETTAMENTE!. */
} BP_SLOT;

/*!
* \brief La struttura che rappresenta un elemento della lista double-linked del
* Buffer di Playout di Rete.
*
* Si tratta di un elemento del vettore che farà parte del Buffer di Playout di
* Rete. All'interno di esso ogni elememto si riferisce all'elemento del vettore
* del Bufferpool con lo stesso indice.
*
* La lista è doppio linkata attraverso gli indici degli elementi precedente e
* successivo all'interno dello stesso vettore.
*
* \note Il valore corripondente al \c NULL, cioè al delimitatore di lista, è \c
* -1
* */
typedef struct {
	int pktlen; /*!<  Lunghezza effettiva del pacchetto corrispondente. */
	int prev; /*!< Indice del precedente elemento della lista ordinata. */
	int next; /*!< indice del successivo elemento della lista ordinata. */
} poitem;

/*!
 * \brief La struttura che rappresenta il Buffer di Playout di Rete.
 *
 * \see poinit
 * \see poadd
 * \see podel
 * */
typedef struct {
	BP_SLOT **bufferpool; /*!< Puntatore alla zona di memoria allocata per
				il Bufferpool e definita altrove. \see bpinit
			       */
	poitem pobuff[BP_SLOT_NUM]; /*!< Vettore che manterrà la lista ordinata
				      degli slot contenenti i pacchetti da
				      processare. */
	pthread_mutex_t po_mutex; /*!< Variabile di mutua esclusione per il
				    controllo di accesso alle strutture
				    sensibili del buffer di playout. */
	int pohead; /*!< Testa della lista. */
	int potail; /*!< Coda della lista. */
	uint32 cycles; /*!< Indice che tiene memoria dei cicli del campo \c
			 timestamp di un pacchetto RTP. */
} playout_buff;

/*!
* \brief La struttura per la gestione del Buffer Pool.
*
* \see bpinit
* \see bpkill
* \see bpget
* \see bpfree
* \see bprmv
* */
typedef struct {
	BP_SLOT *bufferpool; /*!< Puntatore alla zona di memoria allocata per
			       il Bufferpool. \see bpinit */
	pthread_mutex_t fl_mutex; /*!< Variabile di mutua esclusione per il
				    controllo di accesso alle strutture
				    sensibili del Bufferpool. */
	int freelist[BP_SLOT_NUM]; /*!< Lista degli slot liberi. */
	int flhead; /*!< Indice della testa della Free List. */
	int flcount; /*!< Contatore degli elementi presenti nella Free List. */
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
/* @} */
