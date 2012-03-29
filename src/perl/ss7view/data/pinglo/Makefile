#!/usr/bin/make -f

all: db.pm

clean:
	rm -f tmp.log db.pm process.log db.csv convert.log

cleaner: clean

real-clean: cleaner

.PHONY: all clean cleaner real-clean

db.csv: convert.pl Makefile
	./convert.pl 2>&1 | tee convert.log

db.pm: process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

%::
	@echo "nothing to do for $@"
