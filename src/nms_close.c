/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2006 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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


#include <nemesi/main.h>
#include <nemesi/decoder.h>

int nms_close(struct rtsp_ctrl *rtsp_ctl, void (*throbber_func)(void *), void *targ)
{
	if( dec_destroy(decoder_tid) )
		nms_printf(NMSML_ERR, "Cannot initialize decoder\n");

	rtsp_close(rtsp_ctl);
	
	if (throbber_func)
		throbber_func(targ);
	else
		rtsp_wait(rtsp_ctl);
		
	return 0;
}
