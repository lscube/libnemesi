
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
	man2html $(man_MANS) | sed '1,2d' > $(man_MANS).html; \
	echo " done";

clean:
	@echo -n "Cleaning...";\
	rm $(man_MANS).ps.gz $(man_MANS).pdf $(man_MANS).html;\
	echo " done"

distclean: clean


install-data-local:
	
	@if test ! -d $(datadir)/nemesi/docs; then\
		mkdir $(datadir)/nemesi/docs;\
	fi
	$(INSTALL_DATA) -C $(man_MANS).ps.gz $(datadir)/nemesi/docs
	$(INSTALL_DATA) -C $(man_MANS).pdf $(datadir)/nemesi/docs
	$(INSTALL_DATA) -C $(man_MANS).html $(datadir)/nemesi/docs

uninstall-local:
	rm -f $(datadir)/nemesi/docs/$(man_MANS).ps.gz
	rm -f $(datadir)/nemesi/docs/$(man_MANS).pdf
	rm -f $(datadir)/nemesi/docs/$(man_MANS).html
	if test -d $(datadir)/nemesi/docs; then\
		rmdir $(datadir)/nemesi/docs;\
	fi

.PHONY: distclean
