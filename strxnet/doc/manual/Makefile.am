
info_TEXINFOS = strxnet.texi libxnet.texi

EXTRA_DIST = gpl.texi lesser.texi fdl.texi

all-local: ps pdf

install-data-local: all-local
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(DOCDIR)/@PACKAGE_TARNAME@-@PACKAGE_VERSION@
	$(INSTALL_DATA) strxnet.pdf $(DESTDIR)$(DOCDIR)/@PACKAGE_TARNAME@-@PACKAGE_VERSION@
	$(INSTALL_DATA) libxnet.pdf $(DESTDIR)$(DOCDIR)/@PACKAGE_TARNAME@-@PACKAGE_VERSION@

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f $(DESTDIR)$(DOCDIR)/@PACKAGE_TARNAME@-@PACKAGE_VERSION@/strxnet.pdf
	rm -f $(DESTDIR)$(DOCDIR)/@PACKAGE_TARNAME@-@PACKAGE_VERSION@/libxnet.pdf

