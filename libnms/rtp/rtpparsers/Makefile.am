libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include


noinst_HEADERS	=	\
			rtpframer.h \
			rtpframers.h

noinst_LTLIBRARIES = librtpframers.la

librtpframers_la_SOURCES =	rtp_def_parser.c \
				rtpframers.c \
				rtp_mpa.c

librtpframers_la_LDFLAGS = -static
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
