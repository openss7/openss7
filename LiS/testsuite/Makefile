LIS_HOME = $(shell pwd)/..
-include ../config.mk
SUBDIRS = $(shell find . -mindepth 1 -maxdepth 1 -follow -type d | grep -v CVS)


all: wrapper

wrapper: wrapper/wrapper.c
	$(MAKE) -C wrapper;


clean:
	$(MAKE) -C wrapper clean;

install:
	install -d $(LIS_ROOT)/usr/LiS/testsuite/config
	install config/default.exp $(LIS_ROOT)/usr/LiS/testsuite/config
	install -d $(LIS_ROOT)/usr/LiS/testsuite/wrapper
	install wrapper/wrapper $(LIS_ROOT)/usr/LiS/testsuite/wrapper/
	install -d $(LIS_ROOT)/usr/LiS/testsuite/strtst
	install strtst/strtst.exp $(LIS_ROOT)/usr/LiS/testsuite/strtst
	install -d $(LIS_ROOT)/usr/LiS/testsuite/netperf
	install netperf/netperf.exp $(LIS_ROOT)/usr/LiS/testsuite/netperf
	install netperf/netperf.diff $(LIS_ROOT)/usr/LiS/testsuite/netperf
	install README	$(LIS_ROOT)/usr/LiS/testsuite/

uninstall:
	- rm -rf $(LIS_ROOT)/usr/LiS/testsuite	