#!/bin/bash -x

dirs="areacodes localcallingguide nanpa neca4 npanxxsource pinglo telcodata"
for d in $dirs; do
	(
		cd $d
		./create.pl 2>&1 | tee create.log
	)
done
