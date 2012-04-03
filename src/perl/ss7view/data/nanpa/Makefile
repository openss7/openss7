

all: db.csv

clean:
	rm -f tmp.log db.pm process.log db.csv ocn.csv convert.log
	rm -f AllNPAs.txt allutlzd.txt

cleaner: clean
	rm -f AllNPAs.mdb

real-clean: cleaner
	rm -f AllNPAs.zip allutlzd.zip

.PHONY: all clean cleaner real-clean

AllNPAs.zip:
	wget -O $@ http://nanpa.com/npa/AllNPAs.zip

AllNPAs.mdb: AllNPAs.zip
	unzip -p $< $@ > $@

AllNPAs.txt: AllNPAs.mdb
	mdb-export -Q "-d\t" '-D%F' $< 'Public NPA Database Table' > $@

allutlzd.zip:
	wget -O $@ http://www.nanpa.com/nanp1/allutlzd.zip

allutlzd.txt: allutlzd.zip
	unzip -p $< $@ | expand > $@

NPANXX.zip:
	wget -O $@ http://cnac.ca/data/NPANXX.zip

NPANXX.csv: NPANXX.zip
	unzip -p $< cygdrive/d/cna/COCode-ESRDData/NPANXX.csv > $@

db.csv ocn.csv rc.csv: convert.pl Makefile NPANXX.zip
	./convert.pl 2>&1 | tee convert.log

db.pm: AllNPAs.mdb allutlzd.zip NPANXX.zip process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

AllNPAs.mdb: Makefile
AllNPAs.txt: Makefile
allutlzd.txt: Makefile

%::
	@echo "nothing to do for $@"
