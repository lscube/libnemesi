/* * 
 *  ./E-gui/nemesi/txtprintf.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:05 $
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

#include <gnome.h>

#include <ourhdr.h>
#include <nemesi/utils.h>

void txtprintf(gpointer text_area)
{
	int n;
	char optstr;
	static int line_begin=0;
	
	while((n=read(UIINPUT_FILENO, &optstr, 1)) > 0) {
		if (optstr == '\r')
			/*
			gtk_text_set_point((GtkText *) text_area, line_begin);
			*/
			gtk_text_backward_delete((GtkText *) text_area,\
					gtk_text_get_point((GtkText *) text_area)-line_begin);
		else {
			if (optstr == '\n')
				line_begin=gtk_text_get_point((GtkText *) text_area);
			gtk_text_insert((GtkText *) text_area, NULL, NULL, NULL, &optstr, n);
		}
	}
}
