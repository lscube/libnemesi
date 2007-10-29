libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtp.la

librtp_la_SOURCES =	rtp_thread.c \
			rtp_buffer.c \
			rtp_session.c \
			rtp_recv.c \
			rtp_transport.c \
			rtp_ssrc_queue.c \
			rtp_payload_type.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
