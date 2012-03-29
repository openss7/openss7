#!/bin/bash

mkdir -p results
for ((i=200;i<1000;i++)); do
	wget -O results/$i.html --post-data='npaReportModel.nasNpaId='$i 'http://nanpa.com/nas/public/npa_query_step2.do?method=displayNpa'
done

