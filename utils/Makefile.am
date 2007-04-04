libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libutils.la

libutils_la_SOURCES =	urltokenize.c \
			strdup.c \
			strstrcase.c \
			strncmpcase.c \
			strcmpcase.c \
			random32.c \
			f2time.c \
			timeval_subtract.c \
			timeval_add.c \
			uiprintf.c \
			uierror.c \
			nmsprint.c \
			nms_header.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
