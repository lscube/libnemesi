nemesiincludedir=$(top_srcdir)/include

lib_LTLIBRARIES = mpglib.la

libdir = $(nemesi_plugindir)

mpglib_la_SOURCES = mpglib.c\
		common.c \
		dct64_i386.c \
		decode_i386.c \
		layer1.c \
		layer2.c \
		layer3.c \
		tabinit.c \
		interface.c \
		VbrTag.c

noinst_HEADERS = common.h \
		dct64_i386.h \
		decode_i386.h \
		huffman.h \
		interface.h \
		l2tables.h \
		layer1.h \
		layer2.h \
		layer3.h \
		mpg123.h \
		mpglib.h \
		tabinit.h \
		VbrTag.h

mpglib_la_LDFLAGS = -module -avoid-version 
#mpglib_la_LIBADD = -lm libmpglib.la # @LIBLTDL@
mpglib_la_LIBADD = -lm # @LIBLTDL@

INCLUDES = -I$(top_srcdir) -I$(nemesiincludedir)
# INCLUDES += @INCLTDL@
