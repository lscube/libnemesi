.NOTPARALLEL: nemesi

nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libutils.la libplugins.la

libutils_la_SOURCES =	urltokenize.c \
			strdup.c \
			strstrcase.c \
			strncmpcase.c \
			strcmpcase.c \
			statustostr.c \
			random32.c \
			timeval_subtract.c \
			timeval_add.c \
			edit_pref.c \
			edit_avail_pref.c \
			add_avail_pref.c \
			rem_avail_pref.c \
			list_prefs.c \
			get_pref.c \
			uiprintf.c \
			uierror.c \
			nmsprint.c \
			pref2ccmask.c \
			nmsheader.c

libutils_la_LDFLAGS = -static

libplugins_la_SOURCES = load_plugins.c \
			unload_plugins.c

libplugins_la_LDFLAGS = -static

bin_PROGRAMS = nemesi

nemesi_SOURCES =	main.c \
			output_init.c \
			output_uninit.c


nemesi_LDADD =	\
		$(top_srcdir)/rtsp/librtsp.la \
		$(top_srcdir)/sdp/libsdp.la \
		$(top_srcdir)/libCCstreaming/libCCstreaming.la \
		$(top_srcdir)/rtcp/librtcp.la \
		$(top_srcdir)/rtp/librtp.la \
		$(top_srcdir)/decoder/libdecoder.la \
		$(top_srcdir)/bufferpool/libbuffpool.la

if NEMESI_HAVE_GTK
nemesi_LDADD +=	$(top_srcdir)/egui/libegui.la
endif

nemesi_LDADD +=	$(top_srcdir)/etui/libetui.la \
		$(top_srcdir)/wsocket/libwsocket.la \
		$(top_srcdir)/src/libplugins.la \
		$(top_srcdir)/output/audio/libaudio.la \
		$(top_srcdir)/output/video/libvideo.la \
		$(top_srcdir)/output/diskwriter/libdiskwriter.la \
		$(top_srcdir)/src/libutils.la \
		$(top_srcdir)/md5/libmd5.la 

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
