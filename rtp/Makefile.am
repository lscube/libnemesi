
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtp.la
librtp_la_SOURCES =	rtp_clean.c \
			rtp.c \
			rtp_thread_create.c \
			init_rtp_sess.c \
			rtp_recv.c \
			rtp_hdr_val_chk.c \
			set_stm_src.c \
			ssrc_check.c \
			init_seq.c \
			update_seq.c \
			rtcp_to_connect.c \
			rtp_transport_set.c \
			rtp_transport_get.c

librtp_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
