
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libetui.la
libetui_la_SOURCES =	ui.c \
			parse_cl.c \
			parse_prompt.c \
			throbber.c \
			get_infos.c

libetui_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
