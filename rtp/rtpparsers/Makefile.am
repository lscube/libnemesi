libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include


noinst_HEADERS	=	\
			rtpparser.h \
			rtpparsers.h \
			rtp_xiph.h \
			rtp_utils.h

noinst_LTLIBRARIES = librtpparsers.la

librtpparsers_la_SOURCES =	\
				rtpparsers.c \
				rtp_mpa.c \
				rtp_mpv.c \
				rtp_utils.c \
				rtp_vorbis.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
