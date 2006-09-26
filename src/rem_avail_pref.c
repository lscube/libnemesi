/* * 
 *  $Id:rem_avail_pref.c 267 2006-01-12 17:19:45Z shawill $
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

int rem_avail_pref(const char *argstr)
{
	char *name, *available, *avail_tok, tmp_argstr[PREF_MAX_NAME_LEN + PREF_MAX_AVAIL_LEN];
	char new_avail[PREF_MAX_AVAIL_LEN] = "";
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
		fprintf(stderr, "\nError, preference \"%s\" not available, ignoring rem_avail command\n", name);
		return 1;
	}
	if ((available = strtok(NULL, " ")) == NULL) {	/* non e stato inserito un valore */
		fprintf(stderr, "\nError, preference available value for \"%s\" missing, ignoring rem_avail command\n",
			name);
		return 1;
	}
	if (((avail_tok = strstr(preferences[i].available, available)) == NULL) ||
	    ((strlen(avail_tok) != strlen(available)) && (*(avail_tok + strlen(available)) != '/'))) {
		fprintf(stderr,
			"\nThe requested value \"%s\" for \"%s\" is not available, ignoring rem_avail command\n",
			available, name);
		return 1;
	}

	if ((avail_tok = strtok(preferences[i].available, "/")) == NULL) {	/* c'era solo quel valore */
		*(new_avail) = '\0';
	} else {
		do {
			if (strcmp(avail_tok, available)) {
				if (strlen(new_avail))
					strcat(new_avail, "/");
				strcat(new_avail, avail_tok);
			}
		} while ((avail_tok = strtok(NULL, "/")) != NULL);
	}

	fprintf(stderr, "\n\"%s\" available value for \"%s\" removed\n\n", available, name);
	strcpy(preferences[i].available, new_avail);
	// if the pref value was that removed then the new value is set to "null"
	if (!strcmp(preferences[i].value, available))
		strcpy(preferences[i].value, "null");

	return 0;
}
