

all: csv

csv: db.csv

pm: db.pm

clean:
	rm -f tmp.log db.pm process.log convert.log
	rm -f db.csv wc.csv wcfix.csv sw.csv swfix.csv sw.csv ocn.csv
	rm -f NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt

real-clean: clean
	rm -f *.log

.PHONY: all clean real-clean csv pm

NECA-4-1.txt: NECA-4-1.pdf
	pdftotext -layout -nopgbrk $< $@

NECA-4-12.txt: NECA-4-12.pdf
	pdftotext -layout -nopgbrk $< $@

NECA-4-44.txt: NECA-4-44.pdf
	pdftotext -layout -nopgbrk $< $@

NECA-4-400.txt: NECA-4-400.pdf
	pdftotext -layout -nopgbrk $< $@

wccor.csv:

swcor.csv:

db.csv sw.csv swfix.csv wc.csv wcfix.csv ocn.csv: swcor.csv wccor.csv convert.pl Makefile NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt
	./convert.pl 2>&1 | tee convert.log

db.pm: NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

NECA-4-1.txt: Makefile
NECA-4-12.txt: Makefile
NECA-4-44.txt: Makefile
NECA-4-400.txt: Makefile

%::
	@echo "nothing to do for $@"
