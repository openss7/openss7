#!/bin/bash

mkdir -p results
for ((i=200;i<1000;i++)); do
	wget -O results/${i}-city.html --post-data='cityToNpaModel.nasNpaId='$i \
	'http://nanpa.com/nas/public/npa_city_query_step2.do?method=displayData'
done
