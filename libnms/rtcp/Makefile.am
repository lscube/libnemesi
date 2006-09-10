libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtcp.la

librtcp_la_SOURCES =	 \
			rtcp_build_rr.c \
			rtcp_build_sdes.c \
			rtcp_clean_events.c \
			rtcp_clean.c \
			rtcp_deschedule.c \
			rtcp_handle_event.c \
			rtcp_hdr_val_chk.c \
			rtcp_interval.c \
			rtcp_parse_app.c \
			rtcp_parse_bye.c \
			rtcp_parse_pkt.c \
			rtcp_parse_rr.c \
			rtcp_parse_sdes.c \
			rtcp_parse_sr.c \
			rtcp_recv.c \
			rtcp_schedule.c \
			rtcp_send_bye.c \
			rtcp_send_rr.c \
			rtcp_set_ssrc_sdes.c \
			rtcp_thread_create.c \
			rtcp.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
