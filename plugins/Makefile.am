LOCAL_PLUGINS_DIR = plugins

EXTRA_DIST = $(LOCAL_PLUGINS_DIR)

all:
	@LIST_SRC_PLUGINS=`find $(top_builddir)/decoder -name \*.so`; \
	echo -n "Copying plugins..."; \
	for PLUGIN in $$LIST_SRC_PLUGINS; do \
		cp $$PLUGIN $(LOCAL_PLUGINS_DIR)/; \
		DIR=`dirname $$PLUGIN`; \
		NAME=`basename $$PLUGIN | cut -d. -f1`; \
		cp $${PLUGIN%%so}la $(LOCAL_PLUGINS_DIR)/; \
		chmod +x $(LOCAL_PLUGINS_DIR)/$$NAME.la; \
	done; \
	echo " done"

clean:
	@LIST_LOCAL_PLUGINS=`find $(LOCAL_PLUGINS_DIR) \( -name \*.so -or -name \*.la \)`; \
	echo -n "Removing plugins..."; \
	for PLUGIN in $$LIST_LOCAL_PLUGINS; do \
		rm $$PLUGIN; \
	done; \
	echo " done"

uninstall-local:
	rm -rf $(DESTDIR)$(nemesi_plugindir)

distclean: clean
maintainer-clean: clean

.PHONY: distclean
