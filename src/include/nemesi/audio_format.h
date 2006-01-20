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
 *  This file is taken from mplayer code and slightly modified
 *  
 * */

/* Defines that AFMT_ stuff */

#include <config.h> /* for native endianness */

#ifdef	HAVE_SYS_SOUNDCARD_H
#include <sys/soundcard.h>	/* For AFMT_* on linux */
#else 
#ifdef HAVE_SOUNDCARD_H
#include <soundcard.h> /* OpenBSD have this instead of <sys/soundcard> */
#endif
#endif

/* standard, old OSS audio formats */
#ifndef AFMT_MU_LAW
#	define AFMT_MU_LAW		0x00000001
#	define AFMT_A_LAW		0x00000002
#	define AFMT_IMA_ADPCM		0x00000004
#	define AFMT_U8			0x00000008
#	define AFMT_S16_LE		0x00000010	/* Little endian signed 16*/
#	define AFMT_S16_BE		0x00000020	/* Big endian signed 16 */
#	define AFMT_S8			0x00000040
#	define AFMT_U16_LE		0x00000080	/* Little endian U16 */
#	define AFMT_U16_BE		0x00000100	/* Big endian U16 */
#endif

#ifndef AFMT_MPEG
#	define AFMT_MPEG		0x00000200	/* MPEG (2) audio */
#endif

#ifndef AFMT_AC3
#	define AFMT_AC3			0x00000400	/* Dolby Digital AC3 */
#endif

/* 32 bit formats (MSB aligned) formats */
#ifndef AFMT_S32_LE
# define AFMT_S32_LE              0x00001000
# define AFMT_S32_BE              0x00002000
#endif


/* native endian formats */
#ifndef	AFMT_S16_NE
# if WORDS_BIGENDIAN
#  define AFMT_S16_NE	AFMT_S16_BE
#  define AFMT_S32_NE	AFMT_S32_BE
# else
#  define AFMT_S16_NE	AFMT_S16_LE
#  define AFMT_S32_NE	AFMT_S32_LE
# endif
#endif

#ifndef AFMT_FLOAT
# define AFMT_FLOAT               0x00004000
#endif

char *audio_format_name(int format);
int audio_format_bits(int format);

