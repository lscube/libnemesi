
man_MANS = nemesi.1

EXTRA_DIST = 	$(man_MANS)

all:
	@echo -n "Creating Postscript..."; \
	groff -mandoc -e -t -Tps $(man_MANS) > $(man_MANS).ps; \
	echo " done";
	@echo -n "Creating Pdf..."; \
	ps2pdf $(man_MANS).ps $(man_MANS).pdf; \
	echo " done"; \
	gzip -9 -f $(man_MANS).ps
	@echo -n "Creating HTML..."; \
	groff -mandoc -e -t -Thtml $(man_MANS) > $(man_MANS).html; \
	echo " done";

CLEANFILES = $(man_MANS).ps.gz $(man_MANS).pdf $(man_MANS).html

NEMESI_DOCS_DIR = $(DESTDIR)@nemesi_docsdir@

install-data-hook:
	cd $(DESTDIR)@mandir@/man1/; \
	ln -s $(man_MANS) g$(man_MANS); \
	cd -
	
uninstall-hook:
	rm $(DESTDIR)@mandir@/man1/g$(man_MANS)
	
install-data-local:
	$(INSTALL) -m 755 -d $(NEMESI_DOCS_DIR)
	$(INSTALL_DATA) $(man_MANS).ps.gz $(NEMESI_DOCS_DIR)
	$(INSTALL_DATA) $(man_MANS).pdf $(NEMESI_DOCS_DIR)
	$(INSTALL_DATA) $(man_MANS).html $(NEMESI_DOCS_DIR)

uninstall-local:
	rm -rf $(NEMESI_DOCS_DIR)
