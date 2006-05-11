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
			timeval_subtract.c \
			timeval_add.c \
			uiprintf.c \
			uierror.c \
			nmsprint.c \
			nms_header.c

libutils_la_LDFLAGS = -static

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
