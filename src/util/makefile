
all: ss72pcap ss7capmerge ss7capsetdir scapident

ss72pcap: ss72pcap.c
	gcc -g -Wall \
	'-DNAME="ss72pcap"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	ss72pcap.c -o $@ -lpcap

ss7capmerge: ss7capmerge.c
	gcc -g -Wall \
	'-DNAME="ss7capmerge"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	ss7capmerge.c -o $@ -lpcap

ss7capsetdir: ss7capsetdir.c
	gcc -g -Wall \
	'-DNAME="ss7capsetdir"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	ss7capsetdir.c -o $@ -lpcap

scapident: scapident.cpp
	g++ -g -std=gnu++98 -Wall \
	-Wno-deprecated \
	'-DNAME="scapident"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	scapident.cpp -o $@ -lpcap

isupcre: isupcre.cpp
	g++ -g -std=gnu++98 -Wall \
	-Wno-deprecated \
	'-DNAME="isupcre"' \
	'-DPACKAGE="openss7"' \
	'-DVERSION="1.1.1.20110510"' \
	-D_GNU_SOURCE \
	isupcre.cpp -o $@

clean:
	rm -fv ss72pcap
	rm -fv ss7capmerge
	rm -fv ss7capsetdir
	rm -fv scapident
	rm -fv isupcre
