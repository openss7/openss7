LIS_HOME = $(shell pwd)/..
-include ../config.mk
SUBDIRS = $(shell find . -mindepth 1 -maxdepth 1 -follow -type d | grep -v CVS)

all:	subdirs

subdirs:
	set -e; \
	for i in $(SUBDIRS); do \
	    $(MAKE) -C $$i; \
	done
	$(MAKE) drivers.o

modules:
	set -e; \
	for i in $(SUBDIRS); do \
	    $(MAKE) -C $$i $@ || echo "No error, just a try."; \
	done

real-clean: clean
	set -e; \
	for i in $(SUBDIRS); do \
	    $(MAKE) -C $$i $@ || echo "No error, just a try."; \
	done

clean:	
	-rm -f drivers.o
	set -e; \
	for i in $(SUBDIRS); do \
	    $(MAKE) -C $$i $@ || echo "No error, just a try."; \
	done

drivers.o: */*.o
	$(LD) -r -o $@ */*.o

FORCE:

