/* * 
 *  ./etui/parse_cl.c: $Revision: 1.1 $ -- $Date: 2002/10/29 12:43:19 $
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

#include <nemesi/etui.h>

int parse_cl(int argc, char **argv, char **urlname)
{
	int ch;
	int skip;

	skip = 0;
	opterr = 0;
	while (((ch = getopt(argc, argv, CL_OPTIONS)) != -1))
		switch (ch) {
		case 'h':	/* help */
			printf("\n%s - %s -- release %s (%s)\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
			printf("Usage: %s [OPTION]... [URLNAME]\n", PROG_NAME);
			printf("\n");
			printf("\t-h\tdisplay this help and exit\n");
			printf("\t-v\tdisplay version information and exit\n\n");
			skip = 1;
			break;
		case 'v':	/* version */
			printf("\n%s - %s -- release %s (%s)\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
			printf("Copyleft 2001 - mancho@cclinf.polito.it\n");
			printf("              - shawill@cclinf.polito.it\n\n");
			skip = 1;
			break;
		case '?':
			if (isprint(optopt))
				fprintf(stderr, "\nUnknown option `-%c'.\n", optopt);
			else
				fprintf(stderr, "\nUnknown option character `\\x%x'.\n", optopt);
			ch = 'h';
			break;
		default:
			return 1;
		}
	if (argc != 1) {
		if ((optind == argc)) {
			if (!skip) {
				printf("\n%s - %s -- release %s (%s)\n", PROG_NAME, PROG_DESCR, VERSION, VERSION_NAME);
				printf("Usage: %s [OPTION]... [URLNAME]\n", PROG_NAME);
				printf("\n");
				printf("\t-h\tdisplay this help and exit\n");
				printf("\t-v\tdisplay version information and exit\n\n");
			}
			return 1;
		} else if (!skip) {
			if ((*urlname = (char *) malloc(sizeof(char) * (strlen(argv[optind]) + 1))) == NULL)
				return 1;
			strcpy(*urlname, argv[optind]);
		}
	}
	return 0;
}
