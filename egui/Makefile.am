
nemesiincludedir = $(top_srcdir)/include

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I$(nemesiincludedir) -I$(top_srcdir)\
	@PACKAGE_CFLAGS@

EXTRA_DIST =	nemesi-gui.glade \
		nemesi-gui.gladep

noinst_LTLIBRARIES = libegui.la

libegui_la_SOURCES =	gui.c \
			gui_throbber.c \
			support.c support.h \
			interface.c interface.h \
			callbacks.c callbacks.h

libegui_la_LDFLAGS = -static
libegui_la_LIBADD = @PACKAGE_LIBS@
# nemesi_gui_LDADD = @PACKAGE_LIBS@

