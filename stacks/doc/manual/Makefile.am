
info_TEXINFOS = openss7.texi

if WITH_XNET
info_TEXINFOS += libxnet.texi
endif

EXTRA_DIST = gpl.texi lesser.texi fdl.texi libxnet.texi $(PSS) $(PDFS) $(HTMLS)

$(PSS): ps
$(PDFS): pdf $(PSS)
$(HTMLS): html

