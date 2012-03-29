
all: db.pm

clean:
	rm -f db.pm
	$(MAKE) -C nanpa clean
	$(MAKE) -C neca4 clean
	$(MAKE) -C areacodes clean
	$(MAKE) -C telcodata clean

cleaner:
	rm -f db.pm
	$(MAKE) -C nanpa cleaner
	$(MAKE) -C neca4 cleaner
	$(MAKE) -C areacodes cleaner
	$(MAKE) -C telcodata cleaner

real-clean:
	rm -f db.pm
	$(MAKE) -C nanpa real-clean
	$(MAKE) -C neca4 real-clean
	$(MAKE) -C areacodes real-clean
	$(MAKE) -C telcodata real-clean

.PHONY: all clean cleaner real-clean

db.pm:
	$(MAKE) -C nanpa db.pm
	$(MAKE) -C neca4 db.pm
	$(MAKE) -C areacodes db.pm
	$(MAKE) -C telcodata db.pm

db-files ocn-files clli-files:
	$(MAKE) -C areacodes `echo $@ | sed 's,-files,.csv,'`
	$(MAKE) -C localcallingguide `echo $@ | sed 's,-files,.csv,'`
	$(MAKE) -C nanpa `echo $@ | sed 's,-files,.csv,'`
	$(MAKE) -C neca4 `echo $@ | sed 's,-files,.csv,'`
	$(MAKE) -C npanxxsource `echo $@ | sed 's,-files,.csv,'`
	$(MAKE) -C pinglo `echo $@ | sed 's,-files,.csv,'`
	$(MAKE) -C telcodata `echo $@ | sed 's,-files,.csv,'`

.PHONY: db-files ocn-files clli-files

db.csv: db-files */db.csv
	./convert.db.pl 2>&1 | tee convert.log

ocn.csv: ocn-files */ocn.csv
	./convert.ocn.pl 2>&1 | tee convert.log

clli.csv: clli-files */clli.csv
	./convert.clli.pl 2>&1 | tee convert.log
