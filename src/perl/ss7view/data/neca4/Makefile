

all: db.csv

clean:
	rm -f tmp.log db.pm process.log db.csv clli.csv convert.log
	rm -f NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt

real-clean: clean

.PHONY: all clean real-clean

NECA-4-1.txt: NECA-4-1.pdf
	pdftotext -layout $<

NECA-4-12.txt: NECA-4-12.pdf
	pdftotext -layout $<

NECA-4-44.txt: NECA-4-44.pdf
	pdftotext -layout $<

NECA-4-400.txt: NECA-4-400.pdf
	pdftotext -layout $<

db.csv clli.csv: convert.pl Makefile NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt
	./convert.pl 2>&1 | tee convert.log

db.pm: NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

NECA-4-1.txt: Makefile
NECA-4-12.txt: Makefile
NECA-4-44.txt: Makefile
NECA-4-400.txt: Makefile

%::
	@echo "nothing to do for $@"
