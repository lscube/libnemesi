
nemesiincludedir = $(top_srcdir)/include

INCLUDES = \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-I$(nemesiincludedir) -I$(top_srcdir)\
	@PACKAGE_CFLAGS@

THROBBER_DIR = throbber
PIXMAP_DIR = pixmaps
	
EXTRA_DIST =	nemesi-gui.glade \
		nemesi-gui.gladep \
		$(THROBBER_DIR) \
		$(PIXMAP_DIR)

noinst_LTLIBRARIES = libegui.la

libegui_la_SOURCES =	gui.c \
			gui_throbber.c gui_throbber.h \
			support.c support.h \
			interface.c interface.h \
			callbacks.c callbacks.h \
			gui_statusbar.c gui_statusbar.h \
			cc_statw.c gui_cc.h \
			gnmsprint.c

libegui_la_LDFLAGS = -static
libegui_la_LIBADD = @PACKAGE_LIBS@
# nemesi_gui_LDADD = @PACKAGE_LIBS@

NEMESI_THROBBERDIR=$(DESTDIR)@nemesi_throbberdir@
NEMESI_PIXMAPDIR=$(DESTDIR)@nemesi_pixmapdir@

install-data-local:
	$(INSTALL) -m 755 -d $(NEMESI_THROBBERDIR)
	$(INSTALL_DATA) $(THROBBER_DIR)/*.png $(NEMESI_THROBBERDIR)
	$(INSTALL) -m 755 -d $(NEMESI_PIXMAPDIR)
	$(INSTALL_DATA) $(PIXMAP_DIR)/*.png $(NEMESI_PIXMAPDIR)

uninstall-local:
	rm -rf $(NEMESI_THROBBERDIR)
	rm -rf $(NEMESI_PIXMAPDIR)
		

