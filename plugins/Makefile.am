LOCAL_PLUGINS_DIR = plugins

EXTRA_DIST = $(LOCAL_PLUGINS_DIR)

all:
	mkdir -p $(LOCAL_PLUGINS_DIR)
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

CLEANFILES = `find $(LOCAL_PLUGINS_DIR) \( -name \*.so -or -name \*.la \)`

uninstall-local:
	rm -rf $(DESTDIR)$(nemesi_plugindir)
