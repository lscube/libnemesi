/* * 
 *  ./src/strncmpcase.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:14 $
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

int strncmpcase(const char *s1, const char *s2, size_t n)
{
	char *str1;
	char *str2;
	int res;
	unsigned int i;

	if ((str1 = (char *) malloc(sizeof(char) * (strlen(s1) + 1))) == NULL) {
			uiprintf("Cannot allocate memory\n");
		exit(1);
	}
	strcpy(str1, s1);

	for (i = 0; i < strlen(str1); i++)
		if (isupper(str1[i]))
			str1[i] = tolower(str1[i]);

	if ((str2 = (char *) malloc(sizeof(char) * (strlen(s2) + 1))) == NULL) {
		uiprintf("Cannot allocate memory\n");
		exit(1);
	}
	strcpy(str2, s2);

	for (i = 0; i < strlen(str2); i++)
		if (isupper(str2[i]))
			str2[i] = tolower(str2[i]);
	res = strncmp(str1, str2, n);
	free(str1);
	free(str2);
	return res;
}
