
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libmd5.la
libmd5_la_SOURCES =	md5c.c
			
EXTRA_DIST = md5.h global.h

libmd5_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
