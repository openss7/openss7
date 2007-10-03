
all: strMIB_tree.txt tables

MIBDIRS=.:/usr/share/snmp/mibs
MIBS=+OPENSS7-STREAMS-MIB

export MIBDIRS
export MIBS

tree: strMIB_tree.txt

strMIB_tree.txt: Makefile OPENSS7-STREAMS-MIB.mib
	snmptranslate -Tp OPENSS7-STREAMS-MIB::strMIB > $@

tables:
	mib2c -c mib2c.scalar.conf OPENSS7-STREAMS-MIB::strMIB
	mib2c -c mfd-makefile.m2m  OPENSS7-STREAMS-MIB::strMIB
	mib2c -c subagent.m2c      OPENSS7-STREAMS-MIB::strMIB
	mib2c -c mib2c.mfd.conf -S mfd_interactive_setup=0 OPENSS7-STREAMS-MIB::strModTable
	mib2c -c mib2c.mfd.conf -S mfd_interactive_setup=0 OPENSS7-STREAMS-MIB::strModInfoTable
	mib2c -c mib2c.mfd.conf -S mfd_interactive_setup=0 OPENSS7-STREAMS-MIB::strModStatTable
	mib2c -c mib2c.mfd.conf -S mfd_interactive_setup=0 OPENSS7-STREAMS-MIB::strApshTable
	mib2c -c mib2c.mfd.conf -S mfd_interactive_setup=0 OPENSS7-STREAMS-MIB::strStatsTable

clean:
	rm -f strMIB_tree.txt
	rm -f str*Table.[coh]*
	rm -f str*Table_data_access.[coh]*
	rm -f str*Table_data_get.[coh]*
	rm -f str*Table_data_set.[coh]*
	rm -f str*Table_enums.[coh]*
	rm -f str*Table_interface.[coh]*
	rm -f str*Table_Makefile
	rm -f str*Table_oids.h*
	rm -f str*Table-README-*.txt
	rm -f str*Table_subagent.[co]*
	rm -f strMIB.[coh]*
	rm -f strMIB_Makefile
	rm -f strMIB_subagent.[co]*


bins:
	make -f strMIB_Makefile || :
	make -f strModTable_Makefile || :
	make -f strModInfoTable_Makefile || :
	make -f strModStatTable_Makefile || :
	make -f strApshTable_Makefile || :
	make -f strStatsTable_Makefile || :
