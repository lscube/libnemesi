libnmsdir = $(top_srcdir)/libnms

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = libffmp3.la

libdir = $(nemesi_plugindir)

libffmp3_la_SOURCES = ffmp3.c


libffmp3_la_LDFLAGS = -module -avoid-version
libffmp3_la_LIBADD = -lm -lavcodec

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir) -I$(libnmsincludedir)
# INCLUDES += @INCLTDL@
