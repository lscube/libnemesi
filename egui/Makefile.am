
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
			callbacks.c callbacks.h

libegui_la_LDFLAGS = -static
libegui_la_LIBADD = @PACKAGE_LIBS@
# nemesi_gui_LDADD = @PACKAGE_LIBS@

install-data-local:
	$(INSTALL) -m 755 -d $(nemesi_throbberdir)
	$(INSTALL_DATA) $(THROBBER_DIR)/*.png $(nemesi_throbberdir)
	$(INSTALL) -m 755 -d $(nemesi_pixmapdir)
	$(INSTALL_DATA) $(PIXMAP_DIR)/*.png $(nemesi_pixmapdir)

uninstall-local:
	rm -rf $(nemesi_throbberdir)
	rm -rf $(nemesi_pixmapdir)
		

