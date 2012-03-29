#!/bin/bash

if [!-f AllNPAs.zip]; then
	wget -O AllNPAs.zip http://nanpa.com/npa/AllNPAs.zip
fi
if [!-f AllNPAs.mdb]; then
	unzip -p AllNPAs.zip AllNPAs.mdb > AllNPAs.mdb
fi
if [!-f AllNPAs.txt]; then
	mdb-export -Q "-d\t" '-D%F' AllNPAs.mdb 'Public NPA Database Table' > AllNPAs.txt
fi
