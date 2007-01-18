.NOTPARALLEL: nemesi

libnmsdir = $(top_builddir)/libnms

libnmsincludedir = $(top_srcdir)/libnms/include
nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libplugins.la

libplugins_la_SOURCES = add_avail_pref.c \
			edit_avail_pref.c \
			edit_pref.c \
			get_pref.c \
			list_prefs.c \
			load_plugins.c \
			pref2ccmask.c \
			rem_avail_pref.c \
			statustostr.c \
			unload_plugins.c

libplugins_la_LDFLAGS = -static

bin_PROGRAMS = nemesi

nemesi_SOURCES =	main.c \
			output_init.c \
			output_uninit.c \
			nms_close.c \
			nms_open.c \
			nms_play.c


nemesi_LDADD =	$(top_builddir)/etui/libetui.la

if NEMESI_HAVE_GTK
nemesi_LDADD +=	$(top_builddir)/egui/libegui.la
endif

nemesi_LDADD += \
		$(top_builddir)/decoder/libdecoder.la \
		$(libnmsdir)/libnemesi.la \
		$(top_builddir)/src/libplugins.la \
		$(top_builddir)/output/audio/libaudio.la \
		$(top_builddir)/output/video/libvideo.la \
		$(top_builddir)/output/diskwriter/libdiskwriter.la
				
# nemesi_LDADD +=	$(top_builddir)/rtsp/librtsp.la \
#		$(top_builddir)/sdp/libsdp.la \
#		$(top_builddir)/libCCstreaming/libCCstreaming.la \
#		$(top_builddir)/rtcp/librtcp.la \
#		$(top_builddir)/rtp/librtp.la \
#		$(top_builddir)/decoder/libdecoder.la \
#		$(top_builddir)/bufferpool/libbuffpool.la \
#		$(top_builddir)/wsocket/libwsocket.la \
#		$(top_builddir)/src/libplugins.la \
#		$(top_builddir)/output/audio/libaudio.la \
#		$(top_builddir)/output/video/libvideo.la \
#		$(top_builddir)/output/diskwriter/libdiskwriter.la \
#		$(top_builddir)/src/libutils.la \
#		$(top_builddir)/md5/libmd5.la 

if NEMESI_EFENCE
nemesi_LDFLAGS = -lefence
endif

INCLUDES = -I$(libnmsincludedir) -I$(nemesiincludedir) -I$(top_srcdir)

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
