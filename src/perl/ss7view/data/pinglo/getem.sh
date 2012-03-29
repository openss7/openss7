#!/bin/bash
mkdir -p results
for ((i=200;i<1000;i++)); do
	if [ ! -f results/$i.html.xz ]; then
		wget -O - http://www.pinglo.com/$i | xz > results/$i.html.xz
	fi
done
