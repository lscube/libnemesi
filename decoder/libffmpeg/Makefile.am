libnmsdir = $(top_srcdir)/libnms

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = libffmpeg.la

libdir = $(nemesi_plugindir)

libffmpeg_la_SOURCES =	ffmpeg.c \
			init_ffmpeg.c

noinst_HEADERS =	ffmpeg.h

libffmpeg_la_LDFLAGS = -module -avoid-version
libffmpeg_la_LIBADD = -lm -lavcodec

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir) -I$(libnmsincludedir)
