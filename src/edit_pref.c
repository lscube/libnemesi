/* * 
 *  $Id:edit_pref.c 267 2006-01-12 17:19:45Z shawill $
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

int edit_pref(const char *argstr)
{
	char *name, *value, *available, tmp_argstr[PREF_MAX_NAME_LEN + PREF_MAX_AVAIL_LEN];
	int i = 0;

	strcpy(tmp_argstr, argstr);
	if ((name = strtok(tmp_argstr, " ")) == NULL) {
		fprintf(stderr, "\nError, preference name not valid\n");
		return 1;
	}
	/* Cerchiamo il nome del parametro da modificare */
	while (strcmp((char *) &preferences[i], PREFS_TABLE_END) && strcmp(preferences[i].name, name))
		i++;
	if (!strcmp((char *) &preferences[i], PREFS_TABLE_END)) {	/* Il nome non esiste */
		fprintf(stderr, "\nError, preference \"%s\" not available, ignoring edit command\n", name);
		return 1;
	}
	if ((value = strtok(NULL, " ")) == NULL) {	/* non e stato inserito un valore */
		fprintf(stderr, "\nError, preference value for \"%s\" missing, ignoring edit command\n", name);
		return 1;
	}
	if (((available = strstr(preferences[i].available, value)) == NULL) ||
	    ((strlen(available) != strlen(value)) && (*(available + strlen(value)) != '/'))) {
		fprintf(stderr, "\nThe requested value \"%s\" for \"%s\" is not available, ignoring edit command\n",
			value, name);
		return 1;
	}

	fprintf(stderr, "\n\"%s\" value for \"%s\" set\n\n", value, name);
	strcpy(preferences[i].value, value);

	return 0;
}
