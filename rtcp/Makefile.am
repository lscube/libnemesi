
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtcp.la
librtcp_la_SOURCES =	rtcp_clean.c \
			rtcp.c \
			rtcp_thread_create.c \
			rtcp_recv.c \
			rtcp_hdr_val_chk.c \
			parse_rtcp_pkt.c \
			parse_rtcp_sr.c \
			parse_rtcp_rr.c \
			parse_rtcp_bye.c \
			parse_rtcp_sdes.c \
			parse_rtcp_app.c \
			set_ssrc_sdes.c \
			rtcp_interval.c \
			rtcp_schedule.c \
			rtcp_deschedule.c \
			handle_rtcp_event.c \
			send_rtcp_rr.c \
			build_rtcp_rr.c \
			build_rtcp_sdes.c \
			send_rtcp_bye.c

librtcp_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
