nemesiincludedir = $(top_srcdir)/include

noinst_LTLIBRARIES = libutils.la libplugins.la

libutils_la_SOURCES =	urltokenize.c \
			strdup.c \
			strstrcase.c \
			strncmpcase.c \
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
			nmserror.c \
			nmsprintf.c

libutils_la_LDFLAGS = -static

libplugins_la_SOURCES = load_plugins.c

libplugins_la_LDFLAGS = -static
libplugins_la_LIBADD = @LIBLTDL@

bin_PROGRAMS = nemesi

nemesi_SOURCES =	main.c \
			init_rtsp_args.c \
			output_init.c \
			output_uninit.c


nemesi_LDADD =	@DLOPEN@ \
		@DLPREOPEN@ \
		$(top_srcdir)/rtsp/librtsp.la \
		$(top_srcdir)/rtcp/librtcp.la \
		$(top_srcdir)/rtp/librtp.la \
		$(top_srcdir)/decoder/libdecoder.la \
		$(top_srcdir)/bufferpool/libbuffpool.la \
		$(top_srcdir)/etui/libetui.la \
		$(top_srcdir)/wsocket/libwsocket.la \
		$(top_srcdir)/src/libplugins.la \
		$(top_srcdir)/output/audio/libaudio.la \
		$(top_srcdir)/output/video/libvideo.la \
		$(top_srcdir)/output/diskwriter/libdiskwriter.la \
		$(top_srcdir)/src/libutils.la \
		$(top_srcdir)/md5/libmd5.la

INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)
INCLUDES += @INCLTDL@
