
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libetui.la
libetui_la_SOURCES =	ui.c \
			parse_cl.c \
			parse_prompt.c \
			seturlname.c \
			throbber.c \
			send_open.c \
			send_play.c \
			send_pause.c \
			send_close.c \
			edit_pref.c \
			list_prefs.c \
			get_infos.c
libetui_la_LDFLAGS = -static
INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
