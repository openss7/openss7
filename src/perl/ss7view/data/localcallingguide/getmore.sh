#!/bin/bash
mkdir -p ratecenters
for ((i=200;i<1000;i++)); do
	if [ ! -f ratecenters/$i.xml.xz ]; then
		wget -O - 'http://localcallingguide.com/xmlrc.php?npa='$i | xz > ratecenters/$i.xml.xz
	fi
done
