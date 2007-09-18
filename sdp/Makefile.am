libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libsdp.la

libsdp_la_SOURCES =	sdp_session_setup.c \
			sdp_media_setup.c \
			sdp_attr.c \
			sdp_session_destroy.c \
			sdp_media_destroy.c \
			sdp_parse_m_descr.c
			
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
