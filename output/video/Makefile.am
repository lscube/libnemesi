libnmsdir = $(top_srcdir)/libnms

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libvideo.la
libvideo_la_SOURCES =	img_format_name.c \
			video_preinit.c \
			video_th.c \
			video_th_start.c \
			video_th_stop.c \
			video_utils.c
if NEMESI_HAVE_SDL
libvideo_la_SOURCES += video_sdl.c
endif


libvideo_la_LDFLAGS = -static
libvideo_la_LIBADD = @SDL_LIBS@
INCLUDES = -I$(libnmsincludedir) -I$(nemesiincludedir) -I$(top_srcdir) @SDL_CFLAGS@
