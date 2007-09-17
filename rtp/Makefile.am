SUBDIRS = rtpparsers

libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtp.la

librtp_la_SOURCES =	rtp_thread.c \
			rtp_fill_buffer.c \
			rtp_fill_buffers.c \
			rtp_get_next_ts.c \
			rtp_get_n_pkt.c \
			rtp_get_pkt.c \
			rtp_get_fps.c \
			rtp_session_init.c \
			rtp_recv.c \
			rtp_rm_pkt.c \
			rtp_transport.c \
			rtp_ssrc_queue.c \
			rtp_payload_type.c

librtp_la_LIBADD = rtpparsers/librtpparsers.la
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
