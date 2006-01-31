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

#include <nemesi/etui.h>
#include <sys/time.h>
#include <sys/types.h>

void throbber(void *arg)
{
	struct rtsp_ctrl *rtsp_ctl = (struct rtsp_ctrl *)arg;
	struct timeval tv;
	char throbber[20][13] = { "[=          ]",
		"[ =         ]",
		"[  =        ]",
		"[   =       ]",
		"[    =      ]",
		"[     =     ]",
		"[      =    ]",
		"[       =   ]",
		"[        =  ]",
		"[         = ]",
		"[          =]",
		"[         = ]",
		"[        =  ]",
		"[       =   ]",
		"[      =    ]",
		"[     =     ]",
		"[    =      ]",
		"[   =       ]",
		"[  =        ]",
		"[ =         ]"
	};

	
	int i=0, j=0;
	
	setbuf(stdout, NULL);
	
	fprintf(stdout, "             ");
	while ( rtsp_is_busy(rtsp_ctl) ) {
		tv.tv_sec=0;
		tv.tv_usec=50000;
		fprintf(stdout, "\b\b\b\b\b\b\b\b\b\b\b\b\b");
		for (j = 0; j < 13; j++)
			fprintf(stdout, "%c", throbber[i][j]);
		if (i++ == 19)
			i = 0;
		select(0, NULL, NULL, NULL, &tv);
	}
	printf(" Done!\n");
}
