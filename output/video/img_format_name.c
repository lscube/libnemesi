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

#include <nemesi/img_format.h>

char *img_format_name(int format)
{
	switch (format) {
	case IMGFMT_RGB1:
		return ("RGB 1-bit");
	case IMGFMT_RGB4:
		return ("RGB 4-bit");
	case IMGFMT_RG4B:
		return ("RGB 4-bit per byte");
	case IMGFMT_RGB8:
		return ("RGB 8-bit");
	case IMGFMT_RGB15:
		return ("RGB 15-bit");
	case IMGFMT_RGB16:
		return ("RGB 16-bit");
	case IMGFMT_RGB24:
		return ("RGB 24-bit");
	case IMGFMT_RGB32:
		return ("RGB 32-bit");
	case IMGFMT_BGR1:
		return ("BGR 1-bit");
	case IMGFMT_BGR4:
		return ("BGR 4-bit");
	case IMGFMT_BG4B:
		return ("BGR 4-bit per byte");
	case IMGFMT_BGR8:
		return ("BGR 8-bit");
	case IMGFMT_BGR15:
		return ("BGR 15-bit");
	case IMGFMT_BGR16:
		return ("BGR 16-bit");
	case IMGFMT_BGR24:
		return ("BGR 24-bit");
	case IMGFMT_BGR32:
		return ("BGR 32-bit");
	case IMGFMT_YVU9:
		return ("Planar YVU9");
	case IMGFMT_IF09:
		return ("Planar IF09");
	case IMGFMT_YV12:
		return ("Planar YV12");
	case IMGFMT_I420:
		return ("Planar I420");
	case IMGFMT_IYUV:
		return ("Planar IYUV");
	case IMGFMT_CLPL:
		return ("Planar CLPL");
	case IMGFMT_Y800:
		return ("Planar Y800");
	case IMGFMT_Y8:
		return ("Planar Y8");
	case IMGFMT_444P:
		return ("Planar 444P");
	case IMGFMT_422P:
		return ("Planar 422P");
	case IMGFMT_411P:
		return ("Planar 411P");
	case IMGFMT_NV12:
		return ("Planar NV12");
	case IMGFMT_NV21:
		return ("Planar NV21");
	case IMGFMT_HM12:
		return ("Planar NV12 Macroblock");
	case IMGFMT_IUYV:
		return ("Packed IUYV");
	case IMGFMT_IY41:
		return ("Packed IY41");
	case IMGFMT_IYU1:
		return ("Packed IYU1");
	case IMGFMT_IYU2:
		return ("Packed IYU2");
	case IMGFMT_UYVY:
		return ("Packed UYVY");
	case IMGFMT_UYNV:
		return ("Packed UYNV");
	case IMGFMT_cyuv:
		return ("Packed CYUV");
	case IMGFMT_Y422:
		return ("Packed Y422");
	case IMGFMT_YUY2:
		return ("Packed YUY2");
	case IMGFMT_YUNV:
		return ("Packed YUNV");
	case IMGFMT_YVYU:
		return ("Packed YVYU");
	case IMGFMT_Y41P:
		return ("Packed Y41P");
	case IMGFMT_Y211:
		return ("Packed Y211");
	case IMGFMT_Y41T:
		return ("Packed Y41T");
	case IMGFMT_Y42T:
		return ("Packed Y42T");
	case IMGFMT_V422:
		return ("Packed V422");
	case IMGFMT_V655:
		return ("Packed V655");
	case IMGFMT_CLJR:
		return ("Packed CLJR");
	case IMGFMT_YUVP:
		return ("Packed YUVP");
	case IMGFMT_UYVP:
		return ("Packed UYVP");
	case IMGFMT_MPEGPES:
		return ("Mpeg PES");
	case IMGFMT_XVMC_MOCO_MPEG2:
		return ("MPEG1/2 Motion Compensation");
	case IMGFMT_XVMC_IDCT_MPEG2:
		return ("MPEG1/2 Motion Compensation and IDCT");
	}
	return ("Unknown");
}
