/* * 
 *  $Id$
 *  
 *  This file is part of NeMeSI
 *
 *  NeMeSI -- NEtwork MEdia Streamer I
 *
 *  Copyright (C) 2001 by
 *  	
 *  	Giampaolo "mancho" Mancini - giampaolo.mancini@polito.it
 *	Francesco "shawill" Varano - francesco.varano@polito.it
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

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <nemesi/cc.h>
#include <nemesi/comm.h>
#include <nemesi/utils.h>

#define CC_PT_MPA 14

static char cc_taginit=0;
static CCTag *cc_tags=NULL;
static char **cc_exts=NULL;

static int cc_tagsinit(void)
{
	// if ( (!cc_headers) && (!(cc_headers=calloc(128, sizeof(char *)))))
	if ( (!cc_tags) && (!(cc_tags=calloc(128, sizeof(CCTag)))))
		return nmsprintf(NMSML_FATAL, "cc_tag: could not alloc static tags\n");
	
	if ( (!cc_exts) && (!(cc_exts=calloc(128, sizeof(char *)))))
		return nmsprintf(NMSML_FATAL, "cc_tag: could not alloc static extensions\n");

	cc_taginit=1;

	return 0;
}

int cc_setag(int pt, CCLicense *license)
{
	if (!license)
		return 0;

	if ( (!cc_taginit) && (cc_tagsinit()) )
		return 1;

	switch (pt) {
		case CC_PT_MPA: // MPA
			cc_id3v2(license, &cc_tags[CC_PT_MPA]);
			// TODO: set extension
			cc_exts[CC_PT_MPA] = strdup("mp3");
			break;
		default:
			break;
	}

	return 0;
}

int cc_getag(int pt, CCTag **tag, char **ext)
{
	if ( (!cc_taginit) && (cc_tagsinit()) )
		return 1;

	*tag = &cc_tags[pt];
	*ext = cc_exts[pt];

	return 0;
}

