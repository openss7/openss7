#!/bin/bash

# a little script to rebuild the package in your working directory

rm -f cscope.*
./autogen.sh
./configure.sh
make clean
make V=0 cscope
cscope -b
make V=0 clean all README
