/* * 
 *  ./src/load_plugins.c: $Revision: 1.2 $ -- $Date: 2002/11/07 12:12:14 $
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

#include <ltdl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <nemesi/rtpptdefs.h>
#include <nemesi/comm.h>
#include <stdlib.h>

#include <config.h>

int load_plugins(void)
{
	int pt;
	lt_dlhandle module = NULL;
	char *path, *ch;
	char *str=NULL;
	const char *error=NULL;
	int (*get_plugin_pt) (void) = NULL;

	DIR *plug_dir;
	struct dirent *dentry;
	struct stat buf;

	struct plugin {
		char *path;
		struct plugin *next;
	};

	struct plugin *plugins, *pp, *ppp;

	plugins=NULL;

	/* Set the module search path. */
	if (! lt_dlinit()) {
		path = getenv(NEMESI_PLUGIN_DIR_ENV);

		if (path != NULL) {
			if (lt_dlsetsearchpath(path))
				return 1;
			uiprintf("NEMESI_PLUGIN_DIR_ENV: %s\n", path);
		} else {
			if(lt_dlsetsearchpath(NEMESI_PLUGIN_DIR_DEFAULT))
				return 1;
			if ((path = (char *) malloc((strlen(NEMESI_PLUGIN_DIR_DEFAULT) + 1) * sizeof(char))) == NULL) {
				fprintf(stderr, "Cannot allocate memory\n");
				return 1;
			}
			strcpy(path, NEMESI_PLUGIN_DIR_DEFAULT);
			uiprintf("NEMESI_PLUGIN_DIR: %s\n", path);
		}
	} else
		return 1;

	if( (plug_dir = opendir(path)) == NULL ){
		fprintf(stderr, "ERROR: Plugins dir %s does not exist...\n", path);
		return 1;
	}
	while ((dentry = readdir(plug_dir)) != NULL) {

		free(str);
		if ((str=(char *)malloc((strlen(path) + strlen(dentry->d_name) + 2)*sizeof(char)))==NULL){
				fprintf(stderr, "Cannot allocate memory\n");
				return 1;
		}
		strcpy(str, path);
		strcat(str, "/");
		strcat(str, dentry->d_name);

		stat(str, &buf);

		if ( (dentry->d_name[0] == '.' ) || S_ISDIR(buf.st_mode))
			continue;
		
		strcpy(str, dentry->d_name);
		if ( (ch=strrchr(str, '.')) )
			*ch=0;
		
		if (plugins == NULL){
			plugins=(struct plugin *)malloc(sizeof(struct plugin));
			plugins->path=(char *)malloc(sizeof(char)*(strlen(str)+1));
			strcpy(plugins->path,str);
			plugins->next=NULL;
		}

		for (pp=plugins; ( pp->next != NULL ) && ( strcmp(pp->path, str) != 0 ); pp=pp->next);
	
		if (pp->next == NULL && strcmp(pp->path, str)){
			pp->next=(struct plugin *)malloc(sizeof(struct plugin));
			pp->next->path=(char *)malloc(sizeof(char)*(strlen(str)+1));
			strcpy(pp->next->path,str);
			pp->next->next=NULL;
		}
	}
	for (pp=plugins; pp != NULL ; pp=pp->next){
		/* Load the module. */
		module = lt_dlopenext(pp->path);

		/* Find the entry point. */
		if (module) {
			uiprintf("Loading Plugin %s: ", pp->path);
			get_plugin_pt = (int (*)()) lt_dlsym(module, "get_plugin_pt");

			/* In principle, run might legitimately be NULL, so
			   I don't use `run == NULL' as an error indicator
			   in general. */
			if (lt_dlerror() != NULL) {
				lt_dlclose(module);
				module = NULL;
				uiprintf("lt_dsym() failed on get_plugin_pt\n");
				continue;
			}
		} else {
			if ((error=lt_dlerror()) != NULL) {
				uiprintf("lt_dlopenext() failed on plugin %s: %s\n", pp->path, error);
			}
			continue;
		}

		/* Call the entry point function. */
		pt = get_plugin_pt();
		if ((pt < 0) || (pt > 127)) {
			uiprintf("Payload Type Unknown\n");
			continue;
		} else {
			if ( decoders[pt] ){
				lt_dlclose(module);
				uiprintf("WARNING! Plugin for RTP Payload Type %d already loaded: skipping...\n", pt);
				continue;
			}
			decoders[pt] = (int (*)()) lt_dlsym(module, "decode");
			if (lt_dlerror() != NULL) {
				lt_dlclose(module);
				module = NULL;
				decoders[pt]=NULL;
				uiprintf("lt_dsym() failed on decode\n");
				return 1;
			}
			if (!rtp_pt_defs[pt].rate)
				rtp_pt_defs[pt].rate=RTP_DEF_CLK_RATE;
			if (!rtp_pt_defs[pt].channels)
				rtp_pt_defs[pt].channels=1;
			uiprintf("Ok! Loaded plugin for RTP Payload Type %d.\n", pt);
		}
	}
	/*
	for (pp=plugins; pp != NULL ; pp=pp->next){
		free(pp->path);
		ppp=pp->next;
		free(pp);
		pp=ppp;
	}
	*/
	pp=plugins;
	while(pp) {
		ppp=pp->next;
		free(pp->path);
		free(pp);
		pp=ppp;
	}
	free(str);
	if (getenv(NEMESI_PLUGIN_DIR_ENV) == NULL)
		free(path);
	return 0;
}
