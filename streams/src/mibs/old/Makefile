
all: tree stubs agent

MIBS=+OPENSS7-STREAMS-MIB
MIBDIRS=..:/usr/share/snmp/mibs
MIBFILES=../OPENSS7-STREAMS-MIB.mib

export MIBS
export MIBDIRS
export MIBFILES

tree: strMIB_tree.txt

strMIB_tree.txt: $(MAKEFILE) $(MIBFILES)
	snmptranslate -Tp -IR strMIB > $@
	
stubs: strMIB_stub.c strMIB_stub.h

strMIB_stub.c strMIB_stub.h: $(MAKEFILE) $(MIBFILES)
	mib2c -c mib2c.old-api.conf -f strMIB_stub strMIB

agent: strMIB_stub_subagent.c strMIB_stub_subagent.h

strMIB_stub_subagent.c strMIB_stub_subagent.h: $(MAKEFILE) $(MIBFILES)
	mib2c -I `pwd`/,/usr/share/snmp/ -c old-subagent.m2c -f strMIB_stub strMIB

clean:
	rm -fv strMIB_stub.c
	rm -fv strMIB_stub.c~
	rm -fv strMIB_stub.h
	rm -fv strMIB_stub.h~
	rm -fv strMIB_stub_subagent.c
	rm -fv strMIB_stub_subagent.c~
	rm -fv strMIB_stub_subagent.h
	rm -fv strMIB_stub_subagent.h~
	rm -fv strMIB_tree.txt
