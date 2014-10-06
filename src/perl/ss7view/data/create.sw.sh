#!/bin/bash -x

dirs="areacodes localcallingguide neca4 npanxxsource pinglo telcodata"
for d in $dirs; do
	(
		cd $d
		./create.sw.pl 2>&1 | tee create.sw.log
	)
done

