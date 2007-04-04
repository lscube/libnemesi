SUBDIRS =	bufferpool \
		wsocket \
		md5 \
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
			include/nemesi/md5.h\
			include/nemesi/types.h\
			include/nemesi/md5global.h\
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
			md5/libmd5.la \
			$(LIBM)

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libnms.pc

#fixme use VERSION
libnemesi_la_LDFLAGS = -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

if NEMESI_SCTP
libnemesi_la_LDFLAGS += -lsctp
endif

EXTRA_DIST = 	config \
		include \
		debian


MAINTAINER_FILES = acinclude.m4 aclocal.m4 config.h.in* configure Makefile.in mkinstalldirs error.log libnms.pc
MAINTAINER_DIRS = config

maintainer-clean-local:
	@for mfile in $(MAINTAINER_FILES); do \
		test -f $$mfile && rm -f $$mfile; \
	done;\
	for mdir in $(MAINTAINER_DIRS); do \
		test -d $$mdir && rm -rf $$mdir; \
	done;\
	find -iname Makefile.in -exec rm \{} \;

svnclean: maintainer-clean
	@SVNREV_DEFINE=`grep -sE '#define +SVNREV +\" *\\\$$ *Rev.* *\\\$$ *\"' svnrev.h`; \
	test -z "$$SVNREV_DEFINE" && SVNREV_DEFINE="#define SVNREV \"\$$Rev\$$\"" || :; \
	echo -e "/* This header is just for keeping track of current SVN revision.\n * If this file is present in the sources tree then this is a development SVN\n * version.\n * `date`\n * */\n\n#ifndef __SVNREV_H\n#define __SVNREV_H\n\n$$SVNREV_DEFINE\n\n#endif // __SVNREV_H\n" > svnrev.h

indent:
	@echo -n "indenting code with Kernighan & Ritchie coding style... "; \
	export VERSION_CONTROL=simple; \
	export SIMPLE_BACKUP_SUFFIX=.ibak; \
	mkdir -p indent-backup && \
	for file in `find -name \*.c -or -name \*.h` ; do \
		indent -kr -i8 -l79 $${file}; \
		cp --parents $${file}$${SIMPLE_BACKUP_SUFFIX} indent-backup && \
		rm $${file}$${SIMPLE_BACKUP_SUFFIX}; \
	done; \
	echo "done."
