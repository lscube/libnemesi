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

#include <nemesi/rtsp.h>

/*! \brief returns NULL terminated array of strings containing the informations on active RTSP session.
 *
 * The last item of the array will be NULL, this will be the indication the there are no more informations.
 * The keyword "session" as an item of this array will indicate the beggining of the description of a session.
 * The keyword "media" as an item of this array will indicate the beggining of the description of a media.
 * Both keywords will be followed by the session or media name, for example:
 *    session session name
 *    ...
 *    <other session info for name>
 *    ...
 *    media media name1
 *    ...
 *    <other media info for name1>
 *    ...
 *    media madia name2
 *    ...
 *    <other media info for name2>
 *    ...
 *    NULL
 *
 * The array of strings will be allocated internally in this function,
 * but it's a task of the function caller to free array when is no more needed.
 * Instead, each string in the array won't need the 'free'.
 * */
// shawill: TODO: modify for protocol abstraction
char **rtsp_infos(struct RTSP_Ctrl *rtsp_ctrl)
{
	// tmp
	struct RTSP_Thread *rtsp_th = (struct RTSP_Thread *)rtsp_ctrl;
	struct RTSP_Session *sess;
	struct RTSP_Medium *med;
	char **str;
	SDP_attr *attr;
	uint32 items=1; /* initialize it to 1 because the array will contain
			    also the terminator (NULL as last item) */
	char **info_array;

	char *sdes[]=SDP_SESSION_FIELDS;
	char *mdes[]=SDP_MEDIA_FIELDS;
	
	sess=rtsp_th->rtsp_queue;
	
	nmsprintf(NMSML_NORM, BLANK_LINE);

	if (!sess){
		nmsprintf(NMSML_NORM, "No Connection!\n\n");
		return 0;
	}

	// count the number of information strings
	while(sess){
		items++; // new session=> space for session name
		med=sess->media_queue;
		for(str=(char **)(sess->info); str < (char **)&(sess->info->attr_list); str++)
			if (*str)
				items++;
		for(attr=sess->info->attr_list; attr; attr=attr->next, items++);
		while (med) {
			items++; // new media=> space for media name
			for(str=(char **)(med->medium_info); str < (char **)&(med->medium_info->attr_list); str++)
				if(*str)
					items++;
			for(attr=med->medium_info->attr_list; attr; attr=attr->next, items++);
			med=med->next;
		}
		sess=sess->next;
	}

	// now we can allocate the array:
	if ( !(info_array=malloc(items * sizeof(char *))) ) {
		nmsprintf(NMSML_FATAL, "Could not alloc memory\n");
		return NULL;
	}
	
	sess=rtsp_th->rtsp_queue;

	while(sess){
		med=sess->media_queue;
		nmsprintf(NMSML_NORM, "---- RTSP Session Infos: %s ----\n", sess->pathname);
		for(str=(char **)(sess->info); str < (char **)&(sess->info->attr_list); str++)
			if (*str)
				nmsprintf(NMSML_ALWAYS, "* %s: %s\n", sdes[str-(char **)(sess->info)], *str);
		for(attr=sess->info->attr_list; attr; attr=attr->next)
			nmsprintf(NMSML_ALWAYS, "%s\n", attr->a);
		while (med) {
			nmsprintf(NMSML_NORM, "\n\t---- RTSP Medium Infos: %s ----\n", med->filename);
			for(str=(char **)(med->medium_info); str < (char **)&(med->medium_info->attr_list); str++)
				if(*str)
					nmsprintf(NMSML_ALWAYS, "\t* %s: %s\n", mdes[str-(char **)(med->medium_info)], *str);
			for(attr=med->medium_info->attr_list; attr; attr=attr->next)
				nmsprintf(NMSML_ALWAYS, "\t* %s\n", attr->a);
			med=med->next;
		}
		sess=sess->next;
	}
	nmsprintf(NMSML_ALWAYS, "\n");

	return 0;
}
