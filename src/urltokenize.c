/* * 
 *  ./src/urltokenize.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:20 $
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

#include <nemesi/utils.h>

int urltokenize(char *urlname, char **host, char **port, char **path)
{
	char *token, *tokenda;

	if ((tokenda = (char *) malloc(sizeof(char) * (strlen(urlname) + 1))) == NULL)
		return 1;
	strcpy(tokenda, urlname);
	if ((token = strstr(tokenda, "://")) != NULL) {
		token = strtok(tokenda, ":");
		if (port != NULL) {
			if ((*port = (char *) malloc(sizeof(char) * (strlen(token) + 1))) == NULL)
				return 1;
			strcpy(*port, token);
		}
		token += strlen(token) + 3;	/* skip *:// */
	} else
		token = tokenda;
	if (strstr(token, ":") != NULL) {
		token = strtok(token, ":");
		if (host != NULL) {
			if ((*host = (char *) malloc(sizeof(char) * (strlen(token) + 1))) == NULL)
				return 1;
			strcpy(*host, token);
		}
		token = strtok(NULL, "/");
		if (port != NULL) {
			if ((*port = (char *) malloc(sizeof(char) * (strlen(token) + 1))) == NULL)
				return 1;
			strcpy(*port, token);
		}
	} else {
		token = strtok(token, "/");
		if (host != NULL) {
			if ((*host = (char *) malloc(sizeof(char) * (strlen(token) + 1))) == NULL)
				return 1;
			strcpy(*host, token);
		}
	}
	token += strlen(token);
	*(token) = '/';
	if (path != NULL) {
		if ((*path = (char *) malloc(sizeof(char) * (strlen(token) + 1))) == NULL)
			return 1;
		strcpy(*path, token);
	}

	free(tokenda);

	return 0;
}
