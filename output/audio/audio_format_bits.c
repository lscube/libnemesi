/* * 
 *  $Id: audio_format_bits.c 48 2003-11-10 16:01:50Z mancho $
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
 *  This file is taken from mplayer code and slightly modified
 *  
 * */

#include <stdio.h>
#include <stdlib.h>

#include <config.h>
#include <nemesi/audio_format.h>

// return number of bits for 1 sample in one channel, or 8 bits for compressed
int audio_format_bits(int format)
{
	switch (format) {
	case AFMT_S16_LE:
	case AFMT_S16_BE:
	case AFMT_U16_LE:
	case AFMT_U16_BE:
		return 16;	//16 bits

/*
  the following two formats are not available with old linux kernel
  headers (e.g. in 2.2.16)
*/
#ifdef AFMT_S32_LE
	case AFMT_S32_LE:
		return 32;
#endif
#ifdef AFMT_S32_BE
	case AFMT_S32_BE:
		return 32;
#endif
	case AFMT_FLOAT:
		return 32;

	case AFMT_MU_LAW:
	case AFMT_A_LAW:
	case AFMT_IMA_ADPCM:
	case AFMT_S8:
	case AFMT_U8:
	case AFMT_MPEG:
	case AFMT_AC3:
	default:
		return 8;	//default 1 byte

	}
	return 8;
}

