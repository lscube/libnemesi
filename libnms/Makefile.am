SUBDIRS =	bufferpool \
		wsocket \
		md5 \
		utils \
		rtp \
		rtcp \
		libCCstreaming \
		sdp \
		rtsp \
		example
		
# SUBDIRS += 	docs

# nemesiincludedir = $(top_srcdir)/include
# libnmsincludedir = $(srcdir)/include

noinst_LTLIBRARIES = libnemesi.la

libnemesi_la_SOURCES =

libnemesi_la_LIBADD =	rtsp/librtsp.la \
			sdp/libsdp.la \
			libCCstreaming/libCCstreaming.la \
			rtcp/librtcp.la \
			rtp/librtp.la \
			bufferpool/libbuffpool.la \
			wsocket/libwsocket.la \
			utils/libutils.la \
			md5/libmd5.la

# libnemesi_la_LDFLAGS = -static
# libnemesi_la_LDFLAGS = -shared
# INCLUDES = -I$(nemesiincludedir) -I$(top_srcdir)

EXTRA_DIST = 	config \
		include \
		debian


install-exec-hook:
	@echo -n "Removing static libraries..."; \
	rm -f $(DESTDIR)$(nemesi_plugindir)/*.a; \
	echo " done.";

dist-hook:
	@for plugin in $(distdir)/plugins/plugins/*; do \
		rm -rf $$plugin; \
	done
	@for svn in `find $(distdir) -iname .svn`; do \
		rm -rf $$svn; \
	done

uninstall-local:
	test x"`ls $(DESTDIR)$(nemesi_libdir)`" = x && rm -rf $(DESTDIR)$(nemesi_libdir) || echo "$(DESTDIR)$(nemesi_libdir): Directory non empty"
	test x"`ls $(DESTDIR)$(nemesi_sharedir)`" = x && rm -rf $(DESTDIR)$(nemesi_sharedir) || echo "$(DESTDIR)$(nemesi_sharedir): Directory non empty"
	
distclean-local:
	@if test -d autom4te.cache; then \
		rm -rf autom4te.cache; \
	fi

MAINTAINER_FILES = acinclude.m4 aclocal.m4 config.h.in* configure Makefile.in mkinstalldirs error.log egui/*.glade.bak egui/*.gladep.bak
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
