
MIBS=+OPENSS7-STREAMS-MIB
MIBDIRS=.:..:/usr/share/snmp/mibs
MIBFILES=../OPENSS7-STREAMS-MIB.mib

export MIBS
export MIBDIRS
export MIBFILES

all:
	./mib2c -I . -c ./mib2c.conf -f strMIB_old strMIB
	mib2c -I . -c mib2c.old-api.conf -f strMIB_new strMIB

storage:
	./mib2c -I . -c ./mib2c.storage.conf strMIB
	./mib2c -I . -c ./mib2c.storage.conf strApshTable

vartypes:
	./mib2c -I . -c ./mib2c.vartypes.conf strMIB

scalars:
	mib2c -c mib2c.scalar.conf -f strMIB_scalars strMIB

subagent:
	mib2c -c subagent.m2c strMIB
	mib2c -c mfd-makefile.m2m strMIB

