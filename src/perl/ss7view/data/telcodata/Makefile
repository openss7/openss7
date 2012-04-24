

all: db.pm

clean:
	rm -f tmp.log db.pm process.log db.csv ocn.csv sw.csv convert.log

cleaner: clean

real-clean: cleaner

.PHONY: all clean cleaner real-clean

db.csv ocn.csv sw.csv: convert.pl Makefile */data.html.xz
	./convert.pl 2>&1 | tee convert.log

db.pm: process.pl ../database.pm Makefile */data.html.xz
	./process.pl 2>&1 | tee process.log

%::
	@echo "nothing to do for $@"
