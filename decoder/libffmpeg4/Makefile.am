nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = libffmpeg4.la

libdir = $(nemesi_plugindir)

libffmpeg4_la_SOURCES =	ffmpeg4.c \
			init_ffmpeg4.c

noinst_HEADERS =	ffmpeg4.h

libffmpeg4_la_LDFLAGS = -module -avoid-version
libffmpeg4_la_LIBADD = -lm -lavcodec

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir) 
