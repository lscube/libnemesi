## Process this file with automake to produce Makefile.in
nemesidir = $(top_srcdir)/..
nemesiincludedir = $(top_srcdir)/../include

INCLUDES = \
	-I$(top_srcdir)/intl -I$(top_srcdir)/nemesi -I$(nemesiincludedir) \
	$(GNOME_INCLUDEDIR)

bin_PROGRAMS = e-gui

e_gui_SOURCES = \
	main.c \
	support.c support.h \
	interface.c interface.h \
	callbacks.c callbacks.h

e_gui_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) $(INTLLIBS) ../nemesi/libegui_nemesi.la $(nemesidir)/rtsp/librtsp.la $(nemesidir)/rtp/librtp.la $(nemesidir)/rtcp/librtcp.la $(nemesidir)/decoder/libdecoder.la $(nemesidir)/bufferpool/libbuffpool.la $(nemesidir)/etui/libetui.la $(nemesidir)/wsocket/libwsocket.la $(nemesidir)/src/libutils.la $(nemesidir)/md5/libmd5.la $(nemesidir)/src/libplugins.la

