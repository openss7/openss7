#!/usr/bin/make -f

all: db.pm

clean:
	rm -f tmp.log db.pm process.log db.csv ocn.csv clli.csv convert.log

cleaner: clean

real-clean: cleaner

.PHONY: all clean cleaner real-clean

db.csv ocn.csv clli.csv rc.csv: convert.pl Makefile results/*.A.xml.xz
	./convert.pl 2>&1 | tee convert.log

db.pm: process.pl ../database.pm Makefile results/*.A.xml.xz
	./process.pl 2>&1 | tee tmp.log

%::
	@echo "nothing to do for $@"
