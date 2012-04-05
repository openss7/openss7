#!/usr/bin/make -f

all: csv

XLSFILES = 	Abilene_NIS.xls \
		Amarillo_NIS.xls \
		ATT_West_CA_Tandem_and_subtending_end_officies__03-25-08.xls \
		Austin_NIS.xls \
		Brownsville_NIS.xls \
		CLLI_Information.xls \
		Corpus_Christi_NIS.xls \
		CT_Reverse_NIS.xls \
		Dallas_NIS.xls \
		El_Paso_NIS.xls \
		Fayetteville-FtSmith_LATA_526_NIS.xls \
		Ft_Worth_NIS.xls \
		Houston-Beaumont_NIS.xls \
		Kansas_City.xls \
		Kansas.xls \
		Littlerock-Jonesboro_LATA_528_NIS.xls \
		Longview_NIS.xls \
		Lubbock.xls \
		MaponicsOnlineStore_NPANXX_2008SampleData.xls \
		Midland.xls \
		NIS_Oklahoma_City.xls \
		NIS_Tulsa.xls \
		PineBluff_LATA_530_NIS.xls \
		San_Antonio.xls \
		Springfield.xls \
		St_Louis.xls \
		Tyler.xls \
		Waco.xls \
		Wichita_Falls.xls

CSVFILES = $(XLSFILES:%.xls=%.csv)

$(CSVFILES): %.csv: %.xls
	py_xls2csv $< | sed -e 's/, ,/,"",/g;s/, ,/,"",/g;s/^,/"",/;s/,$$/,""/;s/", "/","/g;s/\.0"/"/g' >$@

csv: $(CSVFILES)

clean:
	rm -f $(CSVFILES)

.PHONY: all csv clean
