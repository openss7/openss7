
all: db.pm

clean:
	rm -f tmp.log db.pm process.log db.csv convert.log
	rm -f AREACODEWORLD-GOLD-SAMPLE.CSV

cleaner: clean

real-clean: cleaner

.PHONY: all clean cleaner real-clean

AREACODEWORLD-GOLD-SAMPLE.CSV: AREACODEWORLD-GOLD-SAMPLES.zip
	unzip -p $< $@ > $@

db.csv: convert.pl Makefile
	./convert.pl 2>&1 | tee convert.log

db.pm: AREACODEWORLD-GOLD-SAMPLES.zip process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

AREACODEWORLD-GOLD-SAMPLE.CSV: Makefile

%::
	@echo "nothing to do for $@"
