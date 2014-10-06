#!/bin/bash -x

dirs="areacodes localcallingguide nanpa neca4 npanxxsource pinglo telcodata"
for d in $dirs; do
	(
		cd $d
		./convert.pl 2>&1 | tee convert.log
	)
done
