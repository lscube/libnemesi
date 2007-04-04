/* * 
 *  $Id:urltokenize.c 267 2006-01-12 17:19:45Z shawill $
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

// XXX it should parse rtsp://hostname:port/path/to/resource
// XXX should not use strtok!

int urltokenize(char *urlname, char **host, char **port, char **path)
{
	char *token, *tokenda;

	// initialization
	if (port)
		*port = NULL;
	if (host)
		*host = NULL;

	if ((tokenda = (char *) malloc(strlen(urlname) + 1)) == NULL)
		return 1;
	strcpy(tokenda, urlname);
	if ((token = strstr(tokenda, "://")) != NULL) {
		token = strtok(tokenda, ":");
		if (port != NULL) {
			*port = strdup(token);
		}
		token += strlen(token) + 3;	/* skip *:// */
	} else
		token = tokenda;
	if (strstr(token, ":") != NULL) {
		token = strtok(token, ":");
		if (host != NULL) {
			*host = strdup(token);
		}
		token = strtok(NULL, "/");
		if (port != NULL) {
			free(*port);
			*port = strdup(token);
		}
	} else {
		token = strtok(token, "/");
		if (host != NULL) {
			free(*host);
			*host = strdup(token);
		}
	}
	token += strlen(token);
	*(token) = '/';
	if (path != NULL) {
		*path = strdup(token);
	}

	free(tokenda);

	return 0;
}
