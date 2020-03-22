#!/bin/bash

# a little script to rebuild the package in your working directory

rm -f cscope.*
rm -f *.cache
rm -f *.site
./autogen.sh
./configure.sh
#make clean
#make V=0 cscope
#cscope -b
#make V=0 clean all README
#make MODPOST_DEBUG=5 MODPOST_VERBOSE=5 V=0
make V=0
