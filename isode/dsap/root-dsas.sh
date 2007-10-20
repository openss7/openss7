#!/bin/sh

DISHPARENT=$$ export DISHPARENT
if [ $$ -ge 32768 ];
then
    PORT="$$"
else
    PORT="`expr $$ + 32768`"
fi
DISHPROC="127.0.0.1 $PORT" export DISHPROC

if bind ; 
then
    squid
else
    exit 1
fi

rm -f dsaptailor.root-dsas entries

search @ -nosize -singlelevel -filt objectClass=quipuDSA -noseq > entries

sort < entries | while read x; do
    echo "$x"
    showentry "@$x" -type description | sed -e "s%^.*- %%" | while read y; do
	echo "# $y" >> dsaptailor.root-dsas
    done
    echo -n 'dsa_address "' >> dsaptailor.root-dsas
    echo -n `echo "$x" | sed -e "s%^cn=%%"` >> dsaptailor.root-dsas
    echo -n '"	' >> dsaptailor.root-dsas
    showentry "@$x" -type presentationAddress -nokey >> dsaptailor.root-dsas
    echo "" >> dsaptailor.root-dsas
done

rm -f entries

unbind

exit 0
