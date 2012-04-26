
all: rc.csv

clean:
	rm -f db.csv
	rm -f rc.csv db.rc.csv rn.csv
	rm -f AREACODEWORLD-GOLD-SAMPLE.CSV

cleaner: clean
	rm -f convert.log process.rc.log

real-clean: cleaner
	rm -f *.log

.PHONY: all clean cleaner real-clean

AREACODEWORLD-GOLD-SAMPLE.CSV: AREACODEWORLD-GOLD-SAMPLES.zip
	unzip -p $< $@ > $@

db.csv: convert.pl AREACODEWORLD-GOLD-SAMPLES.zip
	./convert.pl 2>&1 | tee convert.log

rc.csv: process.rc.pl db.csv
	./process.rc.pl 2>&1 | tee process.rc.log

%::
	@echo "nothing to do for $@"
