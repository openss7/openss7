#!/usr/bin/make -f

ZIPFILES = 	AG.zip \
		AI.zip \
		allCountries.zip \
		AS.zip \
		BB.zip \
		BM.zip \
		BS.zip \
		CA.zip \
		cities1000.zip \
		cities5000.zip \
		cities15000.zip \
		DM.zip \
		DO.zip \
		GB.zip \
		GD.zip \
		GU.zip \
		hierarchy.zip \
		JM.zip \
		KN.zip \
		KY.zip \
		LC.zip \
		MP.zip \
		MS.zip \
		MX.zip \
		no-country.zip \
		PR.zip \
		SX.zip \
		TC.zip \
		TT.zip \
		userTags.zip \
		US.zip \
		VC.zip \
		VG.zip \
		VI.zip

CSVFILES = 	AG.csv \
		AI.csv \
		AS.csv \
		BB.csv \
		BM.csv \
		BS.csv \
		CA.csv \
		cities1000.csv \
		DM.csv \
		DO.csv \
		GB.csv \
		GD.csv \
		GU.csv \
		JM.csv \
		KN.csv \
		KY.csv \
		LC.csv \
		MP.csv \
		MS.csv \
		MX.csv \
		PR.csv \
		SX.csv \
		TC.csv \
		TT.csv \
		US.csv \
		VC.csv \
		VG.csv \
		VI.csv

all: $(CSVFILES)

.PHONY: all

$(ZIPFILES):
	list="$(ZIPFILES)"; for i in $$list; do \
	     if [ ! -f $$i ]; then \
		     j=`echo "$$i" | sed 's,.*/,,'`; \
		     wget "http://download.geonames.org/export/dump/$$j"; \
	     fi; \
	done

$(CSVFILES): %.csv: %.zip
	list="$(CSVFILES)"; for i in $$list; do \
	     j=`echo "$$i" | sed 's,\.csv,.zip,'`; \
	     if [ -f $$i -a $$j -nt $$i ]; then \
		     rm -fv $$i; \
	     fi; \
	done ; \
	./calculate.pl 2>&1 | tee calculate.log
