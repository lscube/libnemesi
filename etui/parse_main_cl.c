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
#include <nemesi/video_drivers.h>
#include <nemesi/audio_drivers.h>

// int parse_main_cl(int argc, char **argv, NMSOutputHints *output_hints)
int parse_main_cl(int argc, char **argv, NMSCLOptions *cl_opt)
{
	NMSOutputHints *output_hints = cl_opt->output;
	NMSUiHints *ui_hints = cl_opt->ui;
	int ret = 0;
	char usage = 0; // printf usage at the end of function
	// getopt variables
	int ch;
	int option_index = 0;
	struct option long_options[] = { CL_LONG_OPTIONS, {0, 0, 0, 0} };
	long i; // index
	long v; // verbosity
	char *v_err;

	opterr = 0;
	optind = 1;
	while (((ch = getopt_long(argc, argv, ":"CL_OPTIONS, long_options, &option_index)) != -1))
		switch (ch) {
		case 'h':	/* help */
			usage = 1;
			ret = 1; // 2 => print usage() at the end of function
			// return 1;
			break;
		case 'V':	/* version */
			version();
			ret = 1;
			// return 1;
			break;
		case 'v':	/* verbose */
			//! salvo in v il livello di "verbosity" corrente
			v = nmsprintf(-1, NULL);
			if (optarg) {
				if (*optarg == 'v') {
					for(i=0;optarg[i]=='v'; i++);
					if (optarg[i]!='\0') {
						nmserror("Invalid argument to \"%s\" option\n", long_options[option_index].name);
						usage = 1;
						ret = -1;
					}
					v += i+1;
				} else if ( (isdigit(*optarg)) ) {
					v = strtol(optarg, &v_err, 10);
					if ( (*v_err) ) {
						nmserror("Invalid argument to \"%s\" option\n", long_options[option_index].name);
						usage = 1;
						ret = -1;
					}
				} else {
					nmserror("Invalid argument to \"%s\" option\n", long_options[option_index].name);
					usage = 1;
					ret = -1;
				}
			} else if ( (optind<argc) ) {
				i = strtol(argv[optind], &v_err, 10);
				if ( !(*v_err) ) {
					v = i;
					optind++;
				} else
					v++;
			} else
				v++;
			if ( v != nmsprintf(-1, NULL) )
				nmsprintf(v, NULL);
				// nmsprintf(0, "Verbosity level set to %d\n", nmsprintf(v, NULL));
			break;
		case 1: // audio out driver selection
			if ( !strcmp(optarg, "help") ) {
				list_audio_out();
				ret = 1;
			} else
				output_hints->audio = strdup(optarg);
			break;
		case 2: // video out driver selection
			if ( !strcmp(optarg, "help") ) {
				list_video_out();
				ret = 1;
			} else
				output_hints->video = strdup(optarg);
			break;
		case 3: // system buffer msec selection
			output_hints->sysbuff_ms = strtol(optarg, &v_err, 10);
			if ( *v_err ) {
				if ( (*v_err == '.') || (*v_err == ',') )
					nmserror("Argument to \"%s\" option must be an integer number of milliseconds", long_options[option_index].name);
				nmserror("Invalid argument to \"%s\" option", long_options[option_index].name);
				usage = 1;
				ret = -1;
			}
			break;
		case 4: // use graphical user interface
			ui_hints->gui = 1;
			// not break, because in gui mode we don't want status string on terminal
		case 5: // do not show buffers status and elapsed time
			nmsstatusprintf(NO_STATUS, NULL);
			break;
		case 6: // use textual user interface
			ui_hints->gui = 0;
			break;
		case ':':
			nmserror("Missing argument for option \"%s\"\n", argv[optind-1]);
			usage = 1;
			ret = -1;
			break;
		case '?':
			if (isprint(optopt))
				nmserror("Unknown option `-%c'.\n", optopt);
			else
				nmserror("Unknown option character `\\x%x'.\n", optopt);
			// ch = 'h';
			usage = 1;
			ret = -1;
			// return -1;
		default:
			break;
		}

	if ( usage )
		usage();
	if ( ret < 0 )
		nmserror ("Error parsing command line... exit\n");
	if (optind < argc) {
		if ( argc == optind+1 ) {
			if ( !(ui_hints->url = strdup(argv[optind])) )
				return nmserror("Could not store urlname given in command line");
		} else
			return nmserror("You can specify only one media URL");
	}
	return ret;
}
