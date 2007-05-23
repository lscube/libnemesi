libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libmd5.la
libmd5_la_SOURCES =	md5c.c
			
EXTRA_DIST = md5.h global.h

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
