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

#include <ltdl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h>
#include <nemesi/rtpptdefs.h>
#include <nemesi/comm.h>
#include <stdlib.h>

#include <config.h>

#include <nemesi/utils.h>

int load_plugins(void)
{
	int pt;
	lt_dlhandle module = NULL;
	char *path, *ch;
	char *str=NULL;
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
	if (!lt_dlinit()) {
		path = getenv(NEMESI_PLUGIN_DIR_ENV);

		if (path != NULL) {
			if (lt_dlsetsearchpath(path))
				return 1;
			nmsprintf(1, "NEMESI_PLUGIN_DIR_ENV: %s\n", path);
		} else {
			if(lt_dlsetsearchpath(NEMESI_PLUGIN_DIR_DEFAULT))
				return 1;
			/* use strdup
			if ((path = (char *) malloc((strlen(NEMESI_PLUGIN_DIR_DEFAULT) + 1) * sizeof(char))) == NULL)
				return nmserror("Cannot allocate memory");
			strcpy(path, NEMESI_PLUGIN_DIR_DEFAULT);
			*/
			path = strdup(NEMESI_PLUGIN_DIR_DEFAULT);
			nmsprintf(1,"NEMESI_PLUGIN_DIR: %s\n", path);
		}
	} else
		return 1;

	if( (plug_dir = opendir(path)) == NULL )
		return nmserror("Plugins dir %s does not exist...", path);
	while ((dentry = readdir(plug_dir)) != NULL) {

		free(str);
		if ((str=(char *)malloc((strlen(path) + strlen(dentry->d_name) + 2)*sizeof(char)))==NULL)
			return nmserror("Cannot allocate memory");
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
			/* use strdup
			plugins->path=(char *)malloc(sizeof(char)*(strlen(str)+1));
			strcpy(plugins->path,str);
			*/
			plugins->path = strdup(str);
			plugins->next=NULL;
		}

		for (pp=plugins; ( pp->next != NULL ) && ( strcmp(pp->path, str) != 0 ); pp=pp->next);
	
		if (pp->next == NULL && strcmp(pp->path, str)){
			pp->next=(struct plugin *)malloc(sizeof(struct plugin));
			/* use strdup
			pp->next->path=(char *)malloc(sizeof(char)*(strlen(str)+1));
			strcpy(pp->next->path,str);
			*/
			pp->next->path = strdup(str);
			pp->next->next=NULL;
		}
	}
	for (pp=plugins; pp != NULL ; pp=pp->next) {
		/* Load the module. */
		module = lt_dlopenext(pp->path);

		/* Find the entry point. */
		if (module) {
			nmsprintf(1, "Loading Plugin %s: ", pp->path);
			if (!(get_plugin_pt = (int (*)()) lt_dlsym(module, "get_plugin_pt"))) {
				lt_dlclose(module);
				module = NULL;
				nmsprintf(1,"lt_dsym() failed on get_plugin_pt: %s\n", lt_dlerror());
				continue;
			}
		} else {
			nmserror("lt_dlopenext() failed on plugin %s: %s", pp->path, lt_dlerror());
			continue;
		}

		/* Call the entry point function. */
		pt = get_plugin_pt();
		if ((pt < 0) || (pt > 127)) {
			nmserror("Payload Type Unknown");
			continue;
		} else {
			if ( decoders[pt] ){
				lt_dlclose(module);
				nmsprintf(2, "WARNING! Plugin for RTP Payload Type %d already loaded: skipping...\n", pt);
				continue;
			}
			if (!(decoders[pt] = (int (*)()) lt_dlsym(module, "decode"))) {
				lt_dlclose(module);
				module = NULL;
				decoders[pt]=NULL;
				nmserror("lt_dsym() failed on decode: %s\n", lt_dlerror());
			}
			if (!rtp_pt_defs[pt].rate)
				rtp_pt_defs[pt].rate=RTP_DEF_CLK_RATE;
			if (!rtp_pt_defs[pt].channels)
				rtp_pt_defs[pt].channels=1;
			nmsprintf(1, "Ok! Loaded plugin for RTP Payload Type %d.\n", pt);
		}
	}
	pp=plugins;
	while(pp) {
		ppp=pp->next;
		free(pp->path);
		free(pp);
		pp=ppp;
	}
	free(str);
	closedir(plug_dir);
	if (getenv(NEMESI_PLUGIN_DIR_ENV) == NULL)
		free(path);
	return 0;
}
