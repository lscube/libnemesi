/* * 
 *  ./bufferpool/poinit.c: $Revision: 1.3 $ -- $Date: 2002/11/28 12:00:47 $
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

#include <nemesi/bufferpool.h>

/*!
 * \brief Inizializza il Buffer di Playout.
 *
 * \param po Il puntatore al Buffer di Playout.
 * \param bp Il puntatore il Buffer Pool.
 * \return 0
 * \see poadd
 * \see podel
 * \see bufferpool.h
 * */
int poinit(playout_buff *po, buffer_pool *bp)
{
	po->bufferpool=&(bp->bufferpool);
	po->pohead = po->potail = -1;
	po->cycles = 0;
	return 0;
}
