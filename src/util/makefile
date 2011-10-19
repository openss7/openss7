
all: ss72pcap isupcre

ss72pcap: ss72pcap.c
	gcc -g -Wall \
	'-DNAME="ss7pcap"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	ss72pcap.c -o $@ -lpcap

isupcre: isupcre.cpp
	g++ -g -std=gnu++98 -Wall \
	-Wno-deprecated \
	'-DNAME="isupcre"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	isupcre.cpp -o $@

