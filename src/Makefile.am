.NOTPARALLEL: nemesi

nemesiincludedir = $(top_srcdir)/include
libnmsdir = $(top_srcdir)/libnms

noinst_LTLIBRARIES = libplugins.la

libplugins_la_SOURCES = load_plugins.c \
			unload_plugins.c

libplugins_la_LDFLAGS = -static

bin_PROGRAMS = nemesi

nemesi_SOURCES =	main.c \
				output_init.c \
				output_uninit.c \
				nmsOpen.c


nemesi_LDADD =	$(top_srcdir)/etui/libetui.la

if NEMESI_HAVE_GTK
nemesi_LDADD +=	$(top_srcdir)/egui/libegui.la
endif

nemesi_LDADD += $(libnmsdir)/libnemesi.la\
				$(top_srcdir)/decoder/libdecoder.la \
				$(top_srcdir)/src/libplugins.la \
				$(top_srcdir)/output/audio/libaudio.la \
				$(top_srcdir)/output/video/libvideo.la \
				$(top_srcdir)/output/diskwriter/libdiskwriter.la
				
# nemesi_LDADD +=	$(top_srcdir)/rtsp/librtsp.la \
#		$(top_srcdir)/sdp/libsdp.la \
#		$(top_srcdir)/libCCstreaming/libCCstreaming.la \
#		$(top_srcdir)/rtcp/librtcp.la \
#		$(top_srcdir)/rtp/librtp.la \
#		$(top_srcdir)/decoder/libdecoder.la \
#		$(top_srcdir)/bufferpool/libbuffpool.la \
#		$(top_srcdir)/wsocket/libwsocket.la \
#		$(top_srcdir)/src/libplugins.la \
#		$(top_srcdir)/output/audio/libaudio.la \
#		$(top_srcdir)/output/video/libvideo.la \
#		$(top_srcdir)/output/diskwriter/libdiskwriter.la \
#		$(top_srcdir)/src/libutils.la \
#		$(top_srcdir)/md5/libmd5.la 

if NEMESI_EFENCE
nemesi_LDFLAGS = -lefence
endif

INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)

$(OBJECTS): libtool
libtool: $(LIBTOOL_DEPS)
	$(SHELL) ./config.status --recheck

NEMESI_BIN=@nemesi_bin@
NEMESI_GUI=@nemesi_gui@

install-exec-local:
	@cd $(DESTDIR)$(exec_prefix)/bin; \
	ln -s $(NEMESI_BIN) $(NEMESI_GUI); \
	cd -

uninstall-local:
	rm -f $(DESTDIR)$(exec_prefix)/bin/$(NEMESI_GUI);
