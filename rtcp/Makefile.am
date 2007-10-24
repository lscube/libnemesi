libnmsdir = $(top_srcdir)
libnmsincludedir = $(libnmsdir)/include
# nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = librtcp.la

librtcp_la_SOURCES =	 \
			rtcp_utils.c \
			rtcp_sdes.c \
			rtcp_report.c \
			rtcp_bye.c \
			rtcp_app.c \
			rtcp_recv.c \
			rtcp_events.c \
			rtcp.c

INCLUDES = -I$(libnmsincludedir) -I$(top_srcdir)
