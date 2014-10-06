
all: txt

clean:

cleaner: clean

real-clean: cleaner

.PHONY: all clean cleaner real-clean

SPUBPDFS = 	T-SP-E.118-2011-PDF-E.pdf \
		T-SP-E.164B-2011-PDF-E.pdf \
		T-SP-E.164C-2011-PDF-E.pdf \
		T-SP-E.164D-11-2011-PDF-E.pdf \
		T-SP-E.180-2010-PDF-E.pdf \
		T-SP-E.212A-2010-PDF-E.pdf \
		T-SP-E.212B-2011-PDF-E.pdf \
		T-SP-E.218-2011-PDF-E.pdf \
		T-SP-LT.1-2012-OAS-PDF-E.pdf \
		T-SP-M.1400-2011-PDF-E.pdf \
		T-SP-Q.708A-2011-PDF-E.pdf \
		T-SP-Q.708B-2011-PDF-E.pdf \
		T-SP-T.35-2012-OAS-PDF-E.pdf \
		T-SP-X.121A-2011-PDF-E.pdf \
		T-SP-X.121B-2011-PDF-E.pdf

$(SPUBPDFS):
	wget -O $@ 'http://www.itu.int/dms_pub/itu-t/opb/sp/$@'

SPUBTXTS = $(SPUBPDFS:%.pdf=%.txt)

$(SPUBTXTS): %.txt: %.pdf
	pdftotext -layout -nopgbrk $< $@

txt: $(SPUBTXTS)

.PHONY: txt

ICC_CONTACT_LIST.pdf:
	wget -O $@ 'https://www.neca.org/cms400min/WorkArea/DownloadAsset.aspx?id=6559'

ICC_CONTACT_LIST.txt: ICC_CONTACT_LIST.pdf
	pdftotext -layout -nopgbrk $< $@

locate08.zip:
	wget -O $@ 'http://www.fcc.gov/Bureaus/Common_Carrier/Reports/FCC-State_Link/Locator/locat08.zip'

locate08.xls: locate08.zip
	unzip -p $< 'Table 3_08.xls' >$@

locate08.csv: locate08.xls
	py_xls2csv $< | sed -e 's/, ,/,"",/g;s/, ,/,"",/g;s/^,/"",/;s/,$$/,""/;s/", "/","/g;s/\.0"/"/g' >$@


