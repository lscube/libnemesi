
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libsdp.la
libsdp_la_SOURCES =	sdp_session_setup.c \
			sdp_media_setup.c \
			sdp_set_attr.c \
			sdp_session_close.c
			
libsdp_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
