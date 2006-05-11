/* * 
 *  $Id:add_avail_pref.c 267 2006-01-12 17:19:45Z shawill $
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

int add_avail_pref(const char *argstr)
{
	char *name, *available, tmp_argstr[PREF_MAX_NAME_LEN + PREF_MAX_AVAIL_LEN];
	int i=0;
	
	strcpy(tmp_argstr, argstr);
	if ( (name = strtok(tmp_argstr, " ")) == NULL ) {
		fprintf(stderr, "\nError, preference name not valid\n");
		return 1;
	}
	/* Cerchiamo il nome del parametro da modificare */
	while(strcmp((char*)&preferences[i], PREFS_TABLE_END) && strcmp(preferences[i].name, name))
		i++;
	if ( !strcmp((char*)&preferences[i], PREFS_TABLE_END) ) { /* Il nome non esiste */
		fprintf(stderr, "\nError, preference \"%s\" not available, ignoring add_avail command\n", name);
		return 1;
	}
	if ( (available = strtok(NULL, " ")) == NULL ) { /* non e stato inserito un valore */
		fprintf(stderr, "\nError, preference available value for \"%s\" missing, ignoring add_avail command\n", name);
		return 1;
	}
	if ( (strlen(available) + strlen(preferences[i].available)) > PREF_MAX_AVAIL_LEN ) {
		fprintf(stderr, "\nError, the string non fit\n");
		return 1;
	}

	fprintf(stderr, "\n\"%s\" available value for \"%s\" added\n\n", available, name);
	strcat(preferences[i].available, "/");
	strcat(preferences[i].available, available);

	return 0;
}

