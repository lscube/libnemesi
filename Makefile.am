SUBDIRS =	bufferpool \
		wsocket \
		md5 \
		rtp \
		rtcp \
		rtsp \
		output \
		decoder \
		etui

if NEMESI_HAVE_GTK
SUBDIRS += egui
endif

SUBDIRS += src plugins docs


EXTRA_DIST = 	config \
		include


dist-hook:
	@for plugin in $(distdir)/plugins/plugins/*; do \
		rm -rf $$plugin; \
	done
	@for svn in `find $(distdir) -iname .svn`; do \
		rm -rf $$svn; \
	done

uninstall-local:
	rm -rf $(nemesi_libdir)
	
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
