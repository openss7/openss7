#!/bin/sh
#
# Install streams into the module directory for this version of
# the kernel.  From there a simple "insmod streams" loads it.
#

. config.in

DIR=`pwd`
MODDIR=/lib/modules/$KVER/misc
if [ ! -d $MODDIR ]; then
    mkdir -p $MODDIR
    if [ $? -ne 0 ]; then
        MODDIR=/lib/modules/preferred/misc
        if [ ! -d $MODDIR ]; then
            mkdir -p $MODDIR
            if [ $? -ne 0 ]; then
                exit 1
            fi
        fi
    fi
    echo Created $MODDIR
fi

if [ -f $MODDIR/streams.o ]; then
    rm -f $MODDIR/streams.o
    if [ $? -ne 0 ]; then
	exit 1
    fi
fi

#
# Make a symbolic link from the modules directory back to the LiS build dir
#
cd $MODDIR
if [ -L streams.o ]; then
    rm streams.o
elif [ -f streams.o ]; then
    rm streams.o
fi
ln -s $DIR/streams.o .
