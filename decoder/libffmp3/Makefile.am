nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = libffmp3.la

libdir = $(DESTDIR)$(nemesi_plugindir)

libffmp3_la_SOURCES = ffmp3.c

#noinst_HEADERS = common.h 

libffmp3_la_LDFLAGS = -module -avoid-version
#mpglib_la_LIBADD = -lm libmpglib.la # @LIBLTDL@
libffmp3_la_LIBADD = -lm -lavcodec#/usr/local/lib/libavcodec.so# @LIBLTDL@
#libffmp3_la_LDADD = /usr/local/lib/libavcodec.so# @LIBLTDL@

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir) 
# INCLUDES += @INCLTDL@
