/* * 
 *  $Id$
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

#include <unistd.h>
#include <getopt.h>

#include <nemesi/etui.h>

int parse_ui_cl(int argc, char **argv, char **urlname)
{
	int ch = 'F';
	int option_index = 0;
	struct option long_options[] = { CL_LONG_OPTIONS, {0, 0, 0, 0} };

	if (strlen(CL_UI_OPTIONS)) {
		opterr = 0;
		nmsprintf(0, "optind = %d; ch = %c\n", optind, ch);
		optind = 1;
		while (((ch = getopt_long(argc, argv, CL_OPTIONS , long_options, &option_index)) != -1))
			switch (ch) {
			case '?':
				if (isprint(optopt))
					nmserror("Unknown option `-%c'.\n", optopt);
				else
					nmserror("Unknown option character `\\x%x'.\n", optopt);
				break;
			default:
				// return 1;
				break;
			}
		nmsprintf(0, "optind = %d; ch = %c\n", optind, ch);
	}
	// if (argc != 1) {
	if (optind < argc) {
		if ( argc == optind+1 ) {
			if ( !(*urlname = strdup(argv[optind])) )
				return nmserror("Could not store urlname given in command line");
		} else
			return nmserror("You can specify only one media URL");
	}
	return 0;
}
