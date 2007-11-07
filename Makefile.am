SUBDIRS =	bufferpool \
		utils \
		rtp \
		rtcp \
		creativecommons \
		sdp \
		rtsp \
		parsers \
		. \
		example

ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libnemesi.la

libincludedir = $(includedir)/nemesi
libinclude_HEADERS = 	include/bufferpool.h\
			include/rtpptdefs.h\
			include/cc.h\
			include/rtsp.h\
			include/comm.h\
			include/sdp.h\
			include/version.h\
			include/rtcp.h\
			include/rtp.h\
			include/transport.h

libnemesi_la_SOURCES =

libnemesi_la_LIBADD =	rtsp/librtsp.la \
			sdp/libsdp.la \
			creativecommons/libcc.la \
			rtcp/librtcp.la \
			rtp/librtp.la \
			bufferpool/libbuffpool.la \
			utils/libutils.la \
			parsers/librtpparsers.la

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnemesi.pc

libnemesi_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

EXTRA_DIST = 	config \
		include \
		debian


MAINTAINER_FILES = acinclude.m4 aclocal.m4 config.h.in* configure Makefile.in mkinstalldirs error.log libnemesi.pc
MAINTAINER_DIRS = config

