libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include


noinst_HEADERS	=	\
			rtpparser.h \
			rtpparsers.h

noinst_LTLIBRARIES = librtpparsers.la

librtpparsers_la_SOURCES =	rtp_def_parser.c \
				rtpparsers.c \
				rtp_mpa.c \
				rtp_mpv.c

librtpparsers_la_LDFLAGS = -static
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
