
info_TEXINFOS = libxnet.texi SPG.texi STREAMS.texi porting.texi

EXTRA_DIST = gpl.texi lesser.texi fdl.texi $(PSS) $(PDFS)

#all-local: ps pdf

$(PSS): ps
$(PDFS): pdf $(PSS)

#docdir = @datadir@/doc/@PACKAGE_TARNAME@-@PACKAGE_VERSION@
#
#install-data-local: $(PDFS)
#	@$(NORMAL_INSTALL)
#	$(mkinstalldirs) $(DESTDIR)$(docdir)
#	$(INSTALL_DATA) libxnet.pdf $(DESTDIR)$(docdir)
#	$(INSTALL_DATA) SPG.pdf $(DESTDIR)$(docdir)
#	$(INSTALL_DATA) STREAMS.pdf $(DESTDIR)$(docdir)
#	$(INSTALL_DATA) porting.pdf $(DESTDIR)$(docdir)
#
#uninstall-local:
#	@$(NORMAL_UNINSTALL)
#	rm -f $(DESTDIR)$(docdir)/libxnet.pdf
#	rm -f $(DESTDIR)$(docdir)/SPG.pdf
#	rm -f $(DESTDIR)$(docdir)/STREAMS.pdf
#	rm -f $(DESTDIR)$(docdir)/porting.pdf

