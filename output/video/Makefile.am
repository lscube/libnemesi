nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libvideo.la
libvideo_la_SOURCES =	img_format_name.c \
			video_preinit.c \
			video_sdl.c \
			video_th.c \
			video_th_start.c

libvideo_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
