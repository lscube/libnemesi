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
// #define SDL_ENABLE_LOCKS

//#define BUGGY_SDL //defined by configure

/* MONITOR_ASPECT MUST BE FLOAT */
#define MONITOR_ASPECT 4.0/3.0

#include <stdlib.h>
#include <SDL.h>

#include <nemesi/comm.h>
#include <nemesi/video_drivers.h>
#include <nemesi/video_driver.h>

#ifdef SDL_ENABLE_LOCKS
#define MUTEX_LOCK(mtx, x)	if (SDL_LockMutex(mtx)) { \
					uierror("Cannot lock mutex"); \
					return x; \
				}

#define MUTEX_UNLOCK(mtx, x)	if (SDL_UnlockMutex(mtx)) { \
					uierror("Cannot unlock mutex"); \
					return x; \
				}
#define	SDL_OVR_LOCK(ovr, x)	if (SDL_LockYUVOverlay (ovr)) { \
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
#else // NOT SDL_ENABLE_LOCKS
#define MUTEX_LOCK(mtx, x)
#define MUTEX_UNLOCK(mtx, x)
#define	SDL_OVR_LOCK(orv, x)
#define SDL_OVR_UNLOCK(ovr)
#define SDL_SRF_LOCK(srf, x)
#define SDL_SRF_UNLOCK(srf)
#endif // SDL_ENABLE_LOCKS

//! define this if you want to choose the audio subdevice for SDL lib. According to SDL documentation: "Using these variables isn't recommened and the names and presence of these variables aren't guaranteed from one release to the next". However... they are useful.
#define SDLENV 1

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

#define VBUFFER_SIZE 3

struct sdl_vbuffer {
	SDL_Overlay *overlay[VBUFFER_SIZE];
	SDL_Surface *surface[VBUFFER_SIZE];
	int readpos;
	int writepos;
	int size;
	SDL_mutex *syn;
	SDL_cond *cond_full;
};

static struct sdl_priv_s {
	char driver[8]; //! video driver used by SDL
	SDL_Surface *display;
	// SDL_Overlay *overlay;
	struct sdl_vbuffer *vbuffer;
	int width, height;
	int d_width, d_height;
	uint8 mode; /* RGB or YUV? */
	uint32 format;
	uint32 sdlflags;
#ifdef SDL_ENABLE_LOCKS
	SDL_mutex *syn;
#endif // SDL_ENABLE_LOCKS
} sdl_priv;

//--- Video buffer ---------------//
static struct sdl_vbuffer *new_vbuffer()
{
	int i; // index
	struct sdl_vbuffer *vbuffer;

	if ((vbuffer = malloc(sizeof(struct sdl_vbuffer))) == NULL) {
		uiprintf("SDL: could not alloc memory for video buffer\n");
		return NULL;
	}

	vbuffer->syn = SDL_CreateMutex();
	vbuffer->cond_full = SDL_CreateCond();

	for (i=0;i<VBUFFER_SIZE;i++) {
		vbuffer->overlay[i] = NULL;
		vbuffer->surface[i] = NULL;
	}
	vbuffer->readpos = vbuffer->writepos = vbuffer->size = 0;
	
	return vbuffer;
}

static void free_vbuffer(struct sdl_vbuffer *vbuffer)
{
	int i; // index

	for(i=0;i<VBUFFER_SIZE;i++) {
		if (vbuffer->overlay[i])
			SDL_FreeYUVOverlay(vbuffer->overlay[i]);
		if (vbuffer->surface[i])
			SDL_FreeSurface(vbuffer->surface[i]);
	}

	SDL_DestroyMutex(vbuffer->syn);
	SDL_DestroyCond(vbuffer->cond_full);

	free(vbuffer);
	return;
}
//--- Video buffer ---------------//

static uint32 preinit(const char *arg)
{
	struct sdl_priv_s *priv = &sdl_priv;
	Uint32 subsystem_init;
	Uint32 flags=0;

	// private struct initialization
	priv->display = NULL;
	// priv->overlay = NULL; // XXX to be removed
	priv->vbuffer = NULL;
	priv->width = priv->height = 0;
	priv->sdlflags = 0;
#ifdef SDL_ENABLE_LOCKS
	priv->syn = SDL_CreateMutex();
#endif // SDL_ENABLE_LOCKS

	subsystem_init = SDL_WasInit(SDL_INIT_EVERYTHING);

	if (!(subsystem_init & SDL_INIT_VIDEO))
		flags |= SDL_INIT_VIDEO;
	if (!(subsystem_init & SDL_INIT_TIMER))
		flags |= SDL_INIT_TIMER;

#ifdef SDLENV
	if (arg)
		setenv("SDL_VIDEODRIVER", arg, 1);
#endif // SDLENV

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

	SDL_VideoDriverName(priv->driver, 8);
	uiprintf("SDL: Using driver: %s\n", priv->driver);	

	// TODO: X11 stuff

	return 0;
}

static uint32 config(uint32 width, uint32 height, uint32 d_width, uint32 d_height, \
	       uint8 fullscreen, char *title, uint32 format) 
{
	SDL_Surface *newsurface;
	struct sdl_priv_s *priv = &sdl_priv;

	uint32 flags=0;

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
	uiprintf("SDL: using software surface\n");
#else // SDL_NOHWSURFACE
	flags /*|*/= SDL_HWSURFACE|SDL_HWACCEL;
	uiprintf("SDL: using hardware surface\n");
#endif // SDL_NOHWSURFACE
	// flags |= SDL_NOFRAME; // if we want non frame window
	if (priv->mode != YUV) {
		flags |= SDL_ANYFORMAT;
		uiprintf("SDL: using ANYFORMAT flag\n");
	}
	
	/* SDL can only scale YUV data */
	if(priv->mode == RGB || priv->mode == BGR) {
		d_width = width;
		d_height = height;
	}


	/* TODO: to be removed
	if (priv->overlay)
		SDL_FreeYUVOverlay(priv->overlay);
	priv->overlay = NULL;
	*/// TODO: END to be removed
	
	if (priv->vbuffer)
		free_vbuffer(priv->vbuffer);
	priv->vbuffer = new_vbuffer();

	if (!(newsurface = SDL_SetVideoMode(width, height, 0, flags))) {
		return uierror("SDL_SetVideoMode failed: %s", SDL_GetError());
	}
	// uiprintf("Set Video Mode: w=%d, h=%d\n", width, newheight);
	uiprintf("Set Video Mode: w=%d, h=%d\n", width, height);
	if (priv->display)
		SDL_FreeSurface(priv->display);
	priv->display = newsurface;

	priv->width = width;
	priv->height = height;
	priv->d_width  = d_width ? d_width : width;
	priv->d_height = d_height ? d_height : height;

	priv->sdlflags = flags;

	SDL_WM_SetCaption(title, title);
	MUTEX_UNLOCK(priv->syn, 1);

	return 0;
}

static uint32 get_picture(int w, int h, NMSPicture *pict)
{
	struct sdl_priv_s *priv = &sdl_priv;
	struct sdl_vbuffer *vbuffer = priv->vbuffer;
	// SDL_Rect *rect = (SDL_Rect *)rect_pt;
	// SDL_Overlay *bmp = sdl_priv.overlay;
	SDL_Overlay *bmp = vbuffer->overlay[vbuffer->writepos];
	char must_alloc=0;

	SDL_LockMutex(vbuffer->syn);
	while(vbuffer->size == VBUFFER_SIZE)
		SDL_CondWait(vbuffer->cond_full, vbuffer->syn);
	SDL_UnlockMutex(vbuffer->syn);

	MUTEX_LOCK(priv->syn, 1);

	if (!bmp) // must alloc
		must_alloc = 1;
	else if ((bmp->w != w) || (bmp->h != h)) {
		uiprintf("Freeing old SDL Overlay 1\n");
		SDL_FreeYUVOverlay(bmp);
		must_alloc = 1;
	}
	/* XXX: already freed in config()
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
		// sdl_priv.overlay = bmp;
		vbuffer->overlay[vbuffer->writepos] = bmp;
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
			return uierror("SDL: unsupported format in get_picture");
			break;
	}
	SDL_OVR_LOCK(bmp, 1);

	return 0;
}

static uint32 draw_picture(NMSPicture *pict)
{
	struct sdl_vbuffer *vbuffer = sdl_priv.vbuffer;
	// SDL_Overlay *bmp = sdl_priv.overlay;
	SDL_Overlay *bmp = vbuffer->overlay[vbuffer->writepos++];

	SDL_OVR_UNLOCK(bmp);
	vbuffer->writepos %= VBUFFER_SIZE;
	SDL_LockMutex(vbuffer->syn);
	vbuffer->size++;
	SDL_UnlockMutex(vbuffer->syn);

	return 0;
}

static uint32 update_screen(void)
{
	struct sdl_priv_s *priv = &sdl_priv;
	struct sdl_vbuffer *vbuffer = priv->vbuffer;
	SDL_Overlay *bmp;

	if (vbuffer->size < (VBUFFER_SIZE / 2) + 1) // no frame to show
		return 0;
	MUTEX_LOCK(priv->syn, 1);

	// bmp = sdl_priv.overlay;
	bmp = vbuffer->overlay[vbuffer->readpos++];
	if (!bmp) {
		MUTEX_UNLOCK(priv->syn, 1);
		return 0;
	}
	SDL_SRF_LOCK(priv->display, 1);
	SDL_DisplayYUVOverlay(bmp, &(sdl_priv.display->clip_rect));
	SDL_SRF_UNLOCK(priv->display);

	vbuffer->readpos %= VBUFFER_SIZE;
	SDL_LockMutex(vbuffer->syn);
	if(vbuffer->size == VBUFFER_SIZE)
		SDL_CondSignal(vbuffer->cond_full);
	vbuffer->size--;
	SDL_UnlockMutex(vbuffer->syn);

	MUTEX_UNLOCK(priv->syn, 1);
	
	return 0;
}
static void uninit(void)
{
	free_vbuffer(sdl_priv.vbuffer);
#ifdef SDL_ENABLE_LOCKS
	 SDL_DestroyMutex(sdl_priv.syn);
#endif // SDL_ENABLE_LOCKS
	SDL_QuitSubSystem(SDL_INIT_VIDEO);

	return;
}

