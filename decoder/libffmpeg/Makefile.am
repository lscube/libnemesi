nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = libffmpeg.la

libdir = $(DESTDIR)$(nemesi_plugindir)

libffmpeg_la_SOURCES =	ffmpeg.c \
			init_ffmpeg.c

noinst_HEADERS =	ffmpeg.h

libffmpeg_la_LDFLAGS = -module -avoid-version
libffmpeg_la_LIBADD = -lm -lavcodec #/usr/local/lib/libavcodec.so# @LIBLTDL@

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir) 
