/* * 
 *  ./src/timeval_add.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:14 $
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

#include <nemesi/utils.h>

int timeval_add(struct timeval *res, const struct timeval *x, const struct timeval *y)
{
	res->tv_sec = x->tv_sec+y->tv_sec;
	
	res->tv_usec = x->tv_usec + y->tv_usec;

	while( res->tv_usec > 1000000 ){
		res->tv_usec -= 1000000;
		res->tv_sec++;
	}
	
	return 0;
}

