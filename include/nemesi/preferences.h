/* * 
 *  ./include/nemesi/etui.h: $Revision: 1.1 $ -- $Date: 2003/01/13 16:49:48 $
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

#ifndef __PREFERENCES_H
#define __PREFERENCES_H

#define PREF_MAX_NAME_LEN 8
#define PREF_MAX_VALUE_LEN 8
#define PREF_MAX_AVAIL_LEN 64
#define PREF_MAX_DESCR_LEN 255

#define PREFS_TABLE_END "pref_end"

struct Preference {
	char name[PREF_MAX_NAME_LEN];
	char value[PREF_MAX_VALUE_LEN];
	char available[PREF_MAX_AVAIL_LEN];
	char description[PREF_MAX_DESCR_LEN];
};

extern struct Preference preferences[];

#endif /* __PREFERENCES_H */

