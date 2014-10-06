#!/bin/bash -x

dirs="areacodes localcallingguide nanpa neca4 npanxxsource telcodata"
for d in $dirs; do
	(
		cd $d
		./create.lt.pl 2>&1 | tee create.lt.log
	)
done

dirs="pinglo"
for d in $dirs; do
	(
		cd $d
		./create.sw.pl 2>&1 | tee create.sw.log
	)
done
