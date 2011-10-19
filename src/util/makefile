
all: ss72pcap

ss72pcap: ss72pcap.c
	gcc -Wall \
	'-DNAME="ss7pcap"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	ss72pcap.c -o $@ -lpcap
