libnmsdir = $(top_srcdir)/libnms

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir=$(top_srcdir)/include

noinst_LTLIBRARIES = libmp3.la

libmp3_la_SOURCES = 	common.c \
			dct64_i386.c \
			decode_i386.c \
			layer3.c \
			tabinit.c \
			interface.c \
			huffman.h \
			mpg123.h \
			mpglib.h

libmp3_la_LDFLAGS = -static

lib_LTLIBRARIES = mp3.la

libdir = $(nemesi_plugindir)

mp3_la_SOURCES = mp3.c
mp3_la_LDFLAGS = -module -avoid-version 
mp3_la_LIBADD = -lm libmp3.la # @LIBLTDL@

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir) -I$(libnmsincludedir)
# INCLUDES += @INCLTDL@
