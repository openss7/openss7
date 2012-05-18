

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

txt: NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt

.PHONY: txt

wccor.csv:

swcor.csv:

db.csv sw.csv swfix.csv wc.csv wcfix.csv ocn.csv: swcor.csv wccor.csv convert.pl Makefile NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt
	./convert.pl 2>&1 | tee convert.log

db.pm: NECA-4-1.txt NECA-4-12.txt NECA-4-44.txt NECA-4-400.txt process.pl ../database.pm Makefile
	./process.pl 2>&1 | tee tmp.log

NECA-4-1.pdf: get
NECA-4-12.pdf: get
NECA-4-44.pdf: get
NECA-4-400.pdf: get

get:
	wget -O - 'http://fjallfoss.fcc.gov/cgi-bin/ws.exe/prod/ccb/etfs/webpublic/browse.hts?IdTariff=99&IdLec=43&User_Type=2&IdLec_User=&id_user=' | sed -rn '/^[[:space:]]{28}<tr>$$/,/^[[:space:]]{28}$$/p' | egrep '^[[:space:]]{28}' | sed -r 's,^[[:space:]]{28},,' | awk 'BEGIN{held="";sect=0;date=""}{\
	if ($$0) {\
		if (/<a href/) { } else { \
			gsub(/<[^>]*>/,""); \
			if ($$1~/(Jan|Feb|Mar|Apr|May|Jun|Jul|Aug|Sep|Oct|Nov)/) { \
				if ($$1~/Jan/) { month =  1 } else \
				if ($$1~/Feb/) { month =  2 } else \
				if ($$1~/Mar/) { month =  3 } else \
				if ($$1~/Apr/) { month =  4 } else \
				if ($$1~/May/) { month =  5 } else \
				if ($$1~/Jun/) { month =  6 } else \
				if ($$1~/Jul/) { month =  7 } else \
				if ($$1~/Aug/) { month =  8 } else \
				if ($$1~/Sep/) { month =  9 } else \
				if ($$1~/Oct/) { month = 10 } else \
				if ($$1~/Nov/) { month = 11 } else \
				if ($$1~/Dec/) { month = 12 } \
				date = sprintf("%04d%02d%02d",$$3,month,$$2) \
			} \
		} \
		held = held $$0; \
	} else {\
		sect = 0; \
		if (held~/Administrative Sections 1-11/) { sect =   1 } else \
		if (held~/Wire Center Sections 12-43/)   { sect =  12 } else \
		if (held~/Wire Center Sections 44-64/)   { sect =  44 } else \
		if (held~/Wire Center Sections 400-401/) { sect = 400 } \
		if (sect) { \
			gsub(/.*href="/,"",held); sub(/".*$$/,"",held); \
			held = "wget -nc -O NECA-4-" sect "-" date ".pdf '"'"'http://fjallfoss.fcc.gov/" held "'"'"'"; \
			print held; \
			system(held); \
		} \
		sect = 0; \
		held = ""; \
	} \
}'
	for pat in NECA-4-1 NECA-4-12 NECA-4-44 NECA-4-400; do \
		file=`ls -r $$pat-*.pdf | head -1`; \
		if [ -n "$$file" ]; then \
			ln -vsf $$file $$pat.pdf ; \
		fi ; \
	done

.PHONY: get

%::
	@echo "nothing to do for $@"
