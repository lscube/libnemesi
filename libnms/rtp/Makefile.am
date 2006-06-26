SUBDIRS = rtpparsers

libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtp.la

librtp_la_SOURCES =	rtp_init.c \
			rtp_active_ssrc_queue.c \
			rtp_announce_pt.c \
			rtp_clean.c \
			rtp_dynpt_reg.c \
			rtp_fill_buffer.c \
			rtp_get_next_pt.c \
			rtp_get_next_ts.c \
			rtp_get_n_pkt.c \
			rtp_get_pkt.c \
			rtp.c \
			rtp_thread_create.c \
			rtp_session_init.c \
			rtp_recv.c \
			rtp_rm_pkt.c \
			rtp_hdr_val_chk.c \
			rtp_ssrc_init.c \
			rtp_ssrc_check.c \
			rtp_init_seq.c \
			rtp_update_seq.c \
			rtcp_to_connect.c \
			rtp_transport_set.c \
			rtp_transport_get.c \
			rtp_transport_wrappers.c \
			rtp_next_active_ssrc.c \
			rtpptdefs.c

librtp_la_LDFLAGS = -static
librtp_la_LIBADD = rtpparsers/librtpparsers.la
INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
