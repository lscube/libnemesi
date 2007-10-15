libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtcp.la

librtcp_la_SOURCES =	 \
			rtcp_clean_events.c \
			rtcp_deschedule.c \
			rtcp_handle_event.c \
			rtcp_interval.c \
			rtcp_sdes.c \
			rtcp_report.c \
			rtcp_bye.c \
			rtcp_parse_app.c \
			rtcp_recv.c \
			rtcp_schedule.c \
			rtcp.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
