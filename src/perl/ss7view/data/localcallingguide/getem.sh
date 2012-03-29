#!/bin/bash
mkdir -p results
for ((i=200;i<1000;i++)); do
	if [ ! -f results/$i.xml.xz ]; then
		wget -O - 'http://localcallingguide.com/xmlprefix.php?npa='$i | xz > results/$i.xml.xz
	fi
done
for ((i=200;i<1000;i++)); do
	if [ ! -f results/$i.A.xml.xz ]; then
		wget -O - 'http://localcallingguide.com/xmlprefix.php?npa='$i'&blocks=1' | xz > results/$i.A.xml.xz
	fi
done
