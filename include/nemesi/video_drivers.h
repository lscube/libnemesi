/* * 
 *  $Id: video_drivers.h 48 2003-11-10 16:01:50Z mancho $
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
 *  This file is largely and freely inspired by video_out.h from MPlayer project.
 *  
 * */

#ifndef __VIDEO_DRIVERS_H
#define __VIDEO_DRIVERS_H

#ifndef NMS_GLOBAL_VIDEO_DRIVERS
#define VDRV_EXTERN extern
#else // NMS_GLOBAL_VIDEO_DRIVERS
#define VDRV_EXTERN
#endif // NMS_GLOBAL_VIDEO_DRIVERS

#include <config.h>
#include <nemesi/types.h>
#include <nemesi/img_format.h>
#include <nemesi/video_img.h>

typedef struct {
        /* driver name ("Matrox Millennium G200/G400" */
        const char *name;
        /* short name (for config strings) ("mga") */
        const char *short_name;
        /* author ("Aaron Holtzman <aholtzma@ess.engr.uvic.ca>") */
        const char *author;
        /* any additional comments */
        const char *comment;
} NMSVDrvInfo;

typedef struct {
	NMSVDrvInfo *info;
	/*
	 * Preinitializes driver (real INITIALIZATION)
	 *   arg - currently it's vo_subdevice
	 *   returns: zero on successful initialization, non-zero on error.
	 */
	uint32 (*preinit)(const char *arg);
        /*
         * Initialize (means CONFIGURE) the display driver.
	 * params:
         *   width,height: image source size
	 *   d_width,d_height: size of the requested window size, just a hint
	 *   fullscreen: flag, 0=windowd 1=fullscreen, just a hint
	 *   title: window title, if available
	 *   format: fourcc of pixel format
         * returns : zero on successful initialization, non-zero on error.
         */
        uint32 (*config)(uint32 width, uint32 height, uint32 d_width,
			 uint32 d_height, uint8 fullscreen, char *title,
			 uint32 format);
	/*
	 * allocs a new picture.
	 * params:
	 *	w: width of requested picture
	 *	h: height of requested picture
	 *	pict: the pointer to a NMSPicture struct that will contain data allocated.
	 * returns 1 on error, 0 otherwise
	 */
	uint32 (*get_picture)(int w, int h, NMSPicture *pict);
	/*
	 * This function must be called by decoder after the hole picture has
	 * been written on the allocated NMSPicture, in order to unlock
	 * previously locked data by "get_picture".
	 * params:
	 *    pict: picture to draw. // XXX: For now we don't need it, but in future... (?)
	 * retuns 1 on error, 0 otherwise
	 */
	uint32 (*draw_picture)(NMSPicture *pict);

        /*
         * Display a new RGB/BGR frame of the video to the screen.
         */
        uint32 (*update_screen)(void);
        /*
         * Closes driver. Should restore the original state of the system.
         */
        void (*uninit)(void);
} NMSVFunctions;

#if HAVE_SDL
VDRV_EXTERN NMSVFunctions nms_video_sdl;
#endif

/*
char *vo_format_name(int format);
int vo_init(void);

vo_functions_t* init_best_video_out(char** vo_list);
void list_video_out();

// NULL terminated array of all drivers
extern vo_functions_t* video_out_drivers[];
 */

#undef NMS_GLOBAL_VIDEO_DRIVERS
#undef VDRV_EXTERN

#endif // __VIDEO_DRIVERS_H

