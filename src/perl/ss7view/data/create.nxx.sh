#!/bin/bash -x

dirs="areacodes localcallingguide nanpa neca4 npanxxsource pinglo telcodata"
for d in $dirs; do
	(
		cd $d
		./create.nxx.pl 2>&1 | tee create.nxx.log
	)
done
