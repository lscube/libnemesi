nemesiincludedir = $(top_srcdir)/include

lib_LTLIBRARIES = mp3stub.la

mp3stub_la_SOURCES = mp3stub.c
mp3stub_la_LDFLAGS = -module -avoid-version
mp3stub_la_LIBADD = @LIBLTDL@

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir)
INCLUDES += @INCLTDL@

.PHONY: debug
debug: all
