#!/usr/bin/make -f

all: csv

csv: db.csv ocn.csv sw.csv rc.csv

clean:
	rm -f tmp.log db.pm process.log db.csv osn.csv sw.csv convert.log

cleaner: clean

real-clean: cleaner

.PHONY: all clean cleaner real-clean cvs

db.csv ocn.csv sw.csv rc.csv: convert.pl Makefile */*/*/*.html.xz
	./convert.pl 2>&1 | tee convert.log

%::
	@echo "nothing to do for $@"
