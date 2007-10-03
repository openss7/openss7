
MIBS=+OPENSS7-STREAMS-MIB
MIBDIRS=.:..:/usr/share/snmp/mibs
MIBFILES=../OPENSS7-STREAMS-MIB.mib

export MIBS
export MIBDIRS
export MIBFILES

all:
	mib2c -c mib2c.conf strMIB

scalars:
	mib2c -c mib2c.scalar.conf -f strMIB_scalars strMIB

subagent:
	mib2c -c subagent.m2c strMIB
	mib2c -c mfd-makefile.m2m strMIB

