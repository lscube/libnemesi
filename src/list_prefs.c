/* * 
 *  $Id:list_prefs.c 267 2006-01-12 17:19:45Z shawill $
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

#include <nemesi/preferences.h>

int list_prefs(void)
{
	int i=0;

	fprintf(stderr, "\n\t--- Available preferences are: ---\n\n");
	while(strcmp((char*)&preferences[i], PREFS_TABLE_END)) {
		fprintf(stderr, "Name: %s\n", preferences[i].name);
		fprintf(stderr, "Current Value: %s\n", preferences[i].value);
		fprintf(stderr, "Availables Values: %s\n", preferences[i].available);
		fprintf(stderr, "Description: %s\n\n", preferences[i].description);
		i++;
	}
	
	return 0;
}

