SUBDIRS =	libltdl \
		bufferpool \
		wsocket \
		md5 \
		rtp \
		rtcp \
		rtsp \
		output \
		decoder \
		etui \
		src \
		plugins \
		docs

EXTRA_DIST = 	config \
		include

dist-hook:
	for auxfile in config/*; do \
		if test -f $$auxfile; then \
			cp -p $$auxfile $(distdir)/libltdl; \
		fi; \
	done; \
	for plugin in $(distdir)/plugins/plugins/*; do \
		rm -f $$plugin; \
	done
		
uninstall-local:
	if test -d $(datadir)/nemesi/plugins; then\
		rmdir $(datadir)/nemesi/plugins;\
	fi
	if test -d $(datadir)/nemesi; then\
		rmdir $(datadir)/nemesi;\
	fi
