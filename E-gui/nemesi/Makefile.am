# nemesiincludedir =	$(includedir)/nemesi
#noinst_HEADERS	= 	ourdir.h

nemesiincludedir = $(top_srcdir)/../include

noinst_LTLIBRARIES = libegui_nemesi.la
libegui_nemesi_la_SOURCES =	get_rtsp_args.c \
				init_main.c \
				txtprintf.c \
				gui_play_throbber.c \
				init_rtsp_args.c \
				update_toolbar.c \
				get_egui.c \
				gui_throbber.c \
				gui_buff_dial.c \
				gui_sys_buff_dial.c

libegui_nemesi_la_LDFLAGS = -static

INCLUDES = \
	-I$(top_srcdir)/src -I$(top_srcdir)/nemesi -I$(nemesiincludedir) \
	$(GNOME_INCLUDEDIR)
