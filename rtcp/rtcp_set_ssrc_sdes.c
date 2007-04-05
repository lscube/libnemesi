/* * 
 *  $Id:rtcp_rtcp_set_ssrc_sdes.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/rtcp.h>

int rtcp_set_ssrc_sdes(rtp_ssrc * stm_src, rtcp_sdes_item_t * item)
{
	char *str = ((char **) (&(stm_src->ssrc_sdes)))[item->type];

	if (str != NULL) {
		if (memcmp(str, item->data, item->len) != 0) {
			free(str);
			if ((str =
			     (((char **) (&(stm_src->ssrc_sdes)))[item->
								  type]) =
			     (char *) malloc(item->len + 1)) == NULL)
				return nms_printf(NMSML_FATAL,
						  "Cannot allocate memory!\n");

			memcpy(str, item->data, item->len);
			str[item->len] = 0;
		}

	} else {
		if ((str = ((char **) (&(stm_src->ssrc_sdes)))[item->type] =
		     (char *) malloc(item->len + 1)) == NULL)
			return nms_printf(NMSML_FATAL,
					  "Cannot allocate memory!\n");

		memcpy(str, item->data, item->len);
		str[item->len] = 0;
	}
	return 0;
}