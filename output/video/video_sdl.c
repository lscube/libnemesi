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
 *  This file is largely and freely inspired by vo_sdl.c from MPlayer project.
 *  
 * */

/* define to force software-surface (video surface stored in system memory)*/
#undef SDL_NOHWSURFACE

/* define to enable surface locks, this might be needed on SMP machines */
#undef SDL_ENABLE_LOCKS

//#define BUGGY_SDL //defined by configure

/* MONITOR_ASPECT MUST BE FLOAT */
#define MONITOR_ASPECT 4.0/3.0

#include <stdlib.h>
#include <SDL.h>

#include <nemesi/comm.h>
#include <nemesi/video_drivers.h>
#include <nemesi/video_driver.h>

#define MUTEX_LOCK(mtx, x)	if (SDL_LockMutex(mtx)) { \
					uierror("Cannot lock mutex"); \
					return x; \
				}

#define MUTEX_UNLOCK(mtx, x)	if (SDL_UnlockMutex(mtx)) { \
					uierror("Cannot unlock mutex"); \
					return x; \
				}
#ifdef SDL_ENABLE_LOCKS
#define	SDL_OVR_LOCK(orv, x)	if (SDL_LockYUVOverlay (ovr)) { \
					uierror("SDL: Couldn't lock YUV overlay\n"); \
					return x; \
				}
#define SDL_OVR_UNLOCK(ovr)	SDL_UnlockYUVOverlay (ovr);

#define SDL_SRF_LOCK(srf, x)	if(SDL_MUSTLOCK(srf)) { \
					if(SDL_LockSurface (srf)) { \
						uierror("SDL: Couldn't lock RGB surface\n"); \
						return x; \
					} \
				}

#define SDL_SRF_UNLOCK(srf)	if(SDL_MUSTLOCK(srf)) \
					SDL_UnlockSurface (srf);
#else // SDL_ENABLE_LOCKS
#define	SDL_OVR_LOCK(orv, x)
#define SDL_OVR_UNLOCK(ovr)
#define SDL_SRF_LOCK(srf, x)
#define SDL_SRF_UNLOCK(srf)
#endif // SDL_ENABLE_LOCKS

static NMSVDrvInfo info = {
	"SDL YUV/RGB/BGR renderer (SDL v1.2.5)",
	"sdl",
	"Open Media Streaming Project Team",
	""
};

NMS_LIB_VIDEO(sdl);

#define YUV 0
#define RGB 1
#define BGR 2

static struct sdl_priv_s {
	SDL_Surface *display;
	SDL_Overlay *overlay;
	int width, height;
	int d_width, d_height;
	uint8 mode; /* RGB or YUV? */
	uint32 format;
	uint32 sdlflags;
	SDL_mutex *syn;
} sdl_priv;

static uint32 preinit(const char *arg)
{
	struct sdl_priv_s *priv = &sdl_priv;
	Uint32 subsystem_init;
	Uint32 flags=0;

	// private struct initialization
	priv->display = NULL;
	priv->overlay = NULL;
	priv->width = priv->height = 0;
	priv->sdlflags = 0;
	priv->syn = SDL_CreateMutex();

	subsystem_init = SDL_WasInit(SDL_INIT_EVERYTHING);

	if (!(subsystem_init & SDL_INIT_VIDEO))
		flags |= SDL_INIT_VIDEO;
	if (!(subsystem_init & SDL_INIT_TIMER))
		flags |= SDL_INIT_TIMER;

	if (!flags) {
		uiprintf("SDL Video already initialized\n");
	} else {
		uiprintf("Initializing SDL Video output\n");
		if (subsystem_init) {
			if (SDL_InitSubSystem(flags))
				return uierror("Could not initialize SDL Video");
		} else {
			flags |= SDL_INIT_NOPARACHUTE;
			if (SDL_Init(flags))
				return uierror("Could not initialize SDL Video");
		}
		uiprintf("SDL Video initialized\n");
	}
	// TODO: X11 stuff

	return 0;
}

static uint32 config(uint32 width, uint32 height, uint32 d_width, uint32 d_height, \
	       uint8 fullscreen, char *title, uint32 format) 
{
	SDL_Surface *newsurface;
	struct sdl_priv_s *priv = &sdl_priv;

	uint32 flags=0;
	// int newwidth;
	// int newheight = height;
	// SDL_Rect *rect;

	switch(format){
		case IMGFMT_I420:
			uiprintf("SDL: Mapping I420 to IYUV\n");
			format = SDL_IYUV_OVERLAY;
		case IMGFMT_YV12:
		case IMGFMT_IYUV:
		case IMGFMT_YUY2:
		case IMGFMT_UYVY:
		case IMGFMT_YVYU:
			priv->mode = YUV;
			break;
		case IMGFMT_BGR15:
		case IMGFMT_BGR16:
		case IMGFMT_BGR24:
		case IMGFMT_BGR32:
			priv->mode = BGR;
			break;
		case IMGFMT_RGB15:
		case IMGFMT_RGB16:
		case IMGFMT_RGB24:
		case IMGFMT_RGB32:
			priv->mode = RGB;
			break;
		default:
			return uierror("SDL: Unsupported image format (0x%X)\n",format);
			// return NULL;
			break;
	}
	uiprintf("SDL: Using 0x%X (%s) image format\n", format, img_format_name(format));
	MUTEX_LOCK(priv->syn, 1);
	priv->format = format;
	
	// flags = SDL_HWACCEL;

#ifdef SDL_NOHWSURFACE
	flags /*|*/= SDL_SWSURFACE;
#else // SDL_NOHWSURFACE
	flags /*|*/= SDL_HWSURFACE;
#endif // SDL_NOHWSURFACE
	// flags |= SDL_NOFRAME; // if we want non frame window
	if (priv->mode != YUV)
		flags |= SDL_ANYFORMAT;

	if (priv->overlay)
		SDL_FreeYUVOverlay(priv->overlay);
	priv->overlay = NULL;

	/* ViCoTrA's stuff
	if (priv->display)
		newheight += priv->display->h;
	if (!(rect = new_rect(width, height))) {
		uiprintf("Cannot init new rect");
		return NULL;
	}
	*/

	// if (!(newsurface = SDL_SetVideoMode(width, newheight, 0, flags))) {
	if (!(newsurface = SDL_SetVideoMode(width, height, 0, flags))) {
		return uierror("SDL_SetVideoMode failed: %s", SDL_GetError());
		// return NULL;
	}
	// uiprintf("Set Video Mode: w=%d, h=%d\n", width, newheight);
	uiprintf("Set Video Mode: w=%d, h=%d\n", width, height);
	if (priv->display)
		SDL_FreeSurface(priv->display);
	priv->display = newsurface;

	priv->width = width;
	// priv->height = newheight;
	priv->height = height;
	priv->sdlflags = flags;

	SDL_WM_SetCaption(title, title);
	MUTEX_UNLOCK(priv->syn, 1);

	return 0;
}

static uint32 get_picture(int w, int h, NMSPicture *pict)
{
	struct sdl_priv_s *priv = &sdl_priv;
	// SDL_Rect *rect = (SDL_Rect *)rect_pt;
	SDL_Overlay *bmp = sdl_priv.overlay;
	char must_alloc=0;

	MUTEX_LOCK(priv->syn, 1);

	if (!bmp) // must alloc
		must_alloc = 1;
	/* XXX: already freed in config()
	else if ((rect->w != w) || (rect->h != h)) {
		uiprintf("Freeing old SDL Overlay 1\n");
		SDL_FreeYUVOverlay(bmp);
		must_alloc = 1;
	}
	else if ((bmp->w != priv->width) || (bmp->h != priv->height)) {
		uiprintf("Freeing old SDL Overlay 2\n");
		SDL_FreeYUVOverlay(bmp);
		must_alloc = 1;
	}
	*/

	if (must_alloc) {
		bmp = SDL_CreateYUVOverlay(priv->width, priv->height, priv->format, priv->display);
		uiprintf("Creating new SDL Overlay: w=%d, h=%d\n", \
				priv->width, priv->height);
		sdl_priv.overlay = bmp;
	}
	MUTEX_UNLOCK(priv->syn, 1);

	pict->data[0] = bmp->pixels[0]; //+rect->y*bmp->pitches[0]+rect->x;
	pict->linesize[0] = bmp->pitches[0];
	switch (priv->format) {
		case IMGFMT_YV12:
			pict->data[1] = bmp->pixels[2]; // + rect->y*bmp->pitches[2]/2 + rect->x;
			pict->linesize[1] = bmp->pitches[2];

			pict->data[2] = bmp->pixels[1]; // + rect->y*bmp->pitches[1]/2 + rect->x;
			pict->linesize[2] = bmp->pitches[1];
			break;
		case IMGFMT_I420:
		case IMGFMT_IYUV: // XXX: I don't know if it's correct (/2)
			pict->data[1] = bmp->pixels[1]; // + rect->y*bmp->pitches[1]/2 + rect->x;
			pict->linesize[1] = bmp->pitches[1];

			pict->data[2] = bmp->pixels[2]; // + rect->y*bmp->pitches[2]/2 + rect->x;
			pict->linesize[2] = bmp->pitches[2];
			break;
		default:
			return uierror("SDL: unsupported format in alloc_picture");
			break;
	}
	SDL_OVR_LOCK(bmp, 1);

	return 0;
}

static uint32 draw_picture(NMSPicture *pict)
{
	SDL_Overlay *bmp = sdl_priv.overlay;

	SDL_OVR_UNLOCK(bmp);

	return 0;
}

static uint32 update_screen(void)
{
	struct sdl_priv_s *priv = &sdl_priv;
	SDL_Overlay *bmp;

	MUTEX_LOCK(priv->syn, 1);

	bmp = sdl_priv.overlay;
	if (!bmp) {
		MUTEX_UNLOCK(priv->syn, 1);
		return 0;
	}
	SDL_SRF_LOCK(priv->display, 1);
	SDL_DisplayYUVOverlay(bmp, &(sdl_priv.display->clip_rect));
	SDL_SRF_UNLOCK(priv->display);

	MUTEX_UNLOCK(priv->syn, 1);
	
	return 0;
}
static void uninit(void)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	return;
}

