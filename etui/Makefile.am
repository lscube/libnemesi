libnmsdir = $(top_srcdir)/libnms

libnmsincludedir = $(libnmsdir)/include
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libetui.la
libetui_la_SOURCES =	ui.c \
			parse_cl.c \
			parse_prompt.c \
			throbber.c \
			tui_event.c

libetui_la_LDFLAGS = -static
INCLUDES = -I$(libnmsincludedir) -I$(nemesiincludedir) -I$(top_srcdir)
