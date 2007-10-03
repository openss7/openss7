
all: strmibd

LIBS= -lucdmibs -lucdagent -lsnmp -ldl -lrpm -lrpmio -lrpmdb -lpopt -lz -lcrypto -lm -lsensors -lresolv -lnsl -ldl -lm -lcrypt -lutil -lpthread -lc -lrt

strmibd: strMIB.c
	gcc -D_GNU_SOURCE=1 $(LIBS) -o $@ $<

#	gcc -D_GNU_SOURCE=1 `net-snmp-config --agent-libs` -o $@ $<

#	gcc -D_GNU_SOURCE=1 -lssl -lucdagent -lucdmibs -lsnmp -o $@ $<

#	gcc -D_GNU_SOURCE=1 -lm -ldl -lnsl -lwrap -lcrypto -lssl -lnss_nis -o $@ $<
#	gcc -D_GNU_SOURCE=1 -lm -ldl -lnsl -lwrap -lIIOP -lcrypto -lssl -lnss_nis -lpopt -lrpm -lrpmio -lrpmdb -lucdagent -lucdmibs -lsnmp /usr/lib/libwrap.a -o $@ $<

