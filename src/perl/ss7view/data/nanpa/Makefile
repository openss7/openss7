

all: csv

csv: db.csv special.csv discon.csv

pm: db.pm

clean:
	rm -f tmp.log process.log convert.log
	rm -f db.pm db.csv ocn.csv rc.csv special.csv discon.csv
	rm -f AllNPAs.txt allutlzd.txt NPANXX.csv

cleaner: clean
	rm -f AllNPAs.mdb *.log

real-clean: cleaner
	rm -f AllNPAs.zip allutlzd.zip NPANXX.zip
	rm -f P3Disconnects.xls discon.xls
	rm -f Codes_requiring_special_handling.xls special.xls

.PHONY: all clean cleaner real-clean csv pm

discon.xls:
	wget -O $@ http://nanpa.com/nanp1/P3Disconnects.xls

discon.csv: discon.xls
	py_xls2csv $< | sed -e 's/", "/","/g;s/\.0"/"/g' >$@

special.xls:
	wget -O $@ http://nanpa.com/reports/Codes_requiring_special_handling.xls

special.csv: special.xls
	py_xls2csv $< | sed -e 's/", "/","/g;s/\.0"/"/g' >$@

AllNPAs.zip:
	wget -O $@ http://nanpa.com/npa/AllNPAs.zip

AllNPAs.mdb: AllNPAs.zip
	unzip -p $< $@ >$@

AllNPAs.txt: AllNPAs.mdb
	mdb-export -Q "-d\t" '-D%F' $< 'Public NPA Database Table' >$@

allutlzd.zip:
	wget -O $@ http://www.nanpa.com/nanp1/allutlzd.zip

allutlzd.txt: allutlzd.zip
	unzip -p $< $@ | expand >$@

NPANXX.zip:
	wget -O $@ http://cnac.ca/data/NPANXX.zip

NPANXX.csv: NPANXX.zip
	unzip -p $< cygdrive/d/cna/COCode-ESRDData/NPANXX.csv >$@

db.csv ocn.csv rc.csv: AllNPA.mdb allutlzd.zip NPANXX.zip convert.pl Makefile
	./convert.pl 2>&1 | tee convert.log

db.pm: AllNPAs.mdb allutlzd.zip NPANXX.zip process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

AllNPAs.mdb: Makefile
AllNPAs.txt: Makefile
allutlzd.txt: Makefile

%::
	@echo "nothing to do for $@"
