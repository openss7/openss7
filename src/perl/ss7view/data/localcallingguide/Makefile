#!/usr/bin/make -f

all: csv

csv: db.csv host.csv switches.csv

pm: db.pm

clean:
	rm -f tmp.log db.pm process.log db.csv ocn.csv clli.csv switches.csv host.csv rc.csv
	rm -f convert.log

cleaner: clean
	rm -f *.log

real-clean: cleaner

.PHONY: all clean cleaner real-clean csv pm

db.csv ocn.csv clli.csv rc.csv: host.csv convert.pl Makefile results/*.A.xml.xz
	./convert.pl 2>&1 | tee convert.log

host.csv: convert.clli.pl Makefile switches/*/*/*/*/data.html.xz
	./convert.clli.pl 2>&1 | tee convert.clli.log

switches.csv: clli.csv convert.switches.pl Makefile switches/*/*/data.html.xz
	./convert.switches.pl 2>&1 | tee convert.switches.log

db.pm: process.pl ../database.pm Makefile results/*.A.xml.xz
	./process.pl 2>&1 | tee tmp.log

%::
	@echo "nothing to do for $@"
