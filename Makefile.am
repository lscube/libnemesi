SUBDIRS =	bufferpool \
		wsocket \
		utils \
		rtp \
		rtcp \
		libCCstreaming \
		sdp \
		rtsp \
		. \
		example

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libnemesi.la

libincludedir = $(includedir)/nemesi
libinclude_HEADERS = 	include/nemesi/bufferpool.h\
			include/nemesi/rtpptdefs.h\
			include/nemesi/cc.h\
			include/nemesi/rtsp.h\
			include/nemesi/comm.h\
			include/nemesi/sdp.h\
			include/nemesi/types.h\
			include/nemesi/utils.h\
			include/nemesi/methods.h\
			include/nemesi/version.h\
			include/nemesi/rtcp.h\
			include/nemesi/wsocket.h\
			include/nemesi/rtp.h

libnemesi_la_SOURCES =

libnemesi_la_LIBADD =	rtsp/librtsp.la \
			sdp/libsdp.la \
			libCCstreaming/libCCstreaming.la \
			rtcp/librtcp.la \
			rtp/librtp.la \
			bufferpool/libbuffpool.la \
			wsocket/libwsocket.la \
			utils/libutils.la \
			$(LIBM)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnemesi.pc

#fixme use VERSION
libnemesi_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

EXTRA_DIST = 	config \
		include \
		debian


MAINTAINER_FILES = acinclude.m4 aclocal.m4 config.h.in* configure Makefile.in mkinstalldirs error.log libnemesi.pc
MAINTAINER_DIRS = config

