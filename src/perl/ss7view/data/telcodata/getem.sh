#!/bin/bash
mkdir -p results
for ((i=200;i<1000;i++)); do
	d="results/$i"
	f="$d/data.html.xz";
	[ -d $d ] || mkdir -v -p $d
	if [ ! -f $f ]; then
		wget -O - "http://www.telcodata.us/search-area-code-exchange-detail?npa=$i&exchange=" | xz > $f
	fi
done
