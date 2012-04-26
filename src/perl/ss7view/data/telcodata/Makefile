

all: rc.csv

clean:
	rm -f db.csv
	rm -f rc.csv db.rc.csv rn.csv db.rn.csv

cleaner: clean
	rm -f convert.log process.rc.log

real-clean: cleaner
	rm -f *.log

.PHONY: all clean cleaner real-clean

db.csv: convert.pl results/*/data.html.xz
	./convert.pl 2>&1 | tee convert.log

rc.csv: process.rc.log db.csv
	./process.rc.pl 2>&1 | tee process.rc.log

%::
	@echo "nothing to do for $@"
