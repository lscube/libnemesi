libnmsdir = $(top_srcdir)/libnms
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libutils.la

libutils_la_SOURCES =	urltokenize.c \
			strdup.c \
			strstrcase.c \
			strncmpcase.c \
			strcmpcase.c \
			statustostr.c \
			random32.c \
			timeval_subtract.c \
			timeval_add.c \
			edit_pref.c \
			edit_avail_pref.c \
			add_avail_pref.c \
			rem_avail_pref.c \
			list_prefs.c \
			get_pref.c \
			uiprintf.c \
			uierror.c \
			nmsprint.c \
			pref2ccmask.c \
			nms_header.c

libutils_la_LDFLAGS = -static

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
