libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtsp.la

librtsp_la_SOURCES =	rtsp.c \
                        rtsp_handlers.c \
                        rtsp_internals.c \
                        rtsp_send.c \
                        rtsp_sessions.c \
                        rtsp_state_machine.c \
                        rtsp_transport.c
			
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
