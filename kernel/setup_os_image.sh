#!/usr/bin/bash

BUILDDIR=$1
OSNAME=$2

if ! [ -f "$BUILDDIR/$OSNAME.img" ]; then
    dd if=/dev/zero of=$BUILDDIR/$OSNAME.img bs=4096 count=100000
    sgdisk --clear --new 1:2048:98304 --new 2:100352:799933 -t 1:EF00 -t 2:8300 $BUILDDIR/$OSNAME.img
fi


if ! [ -f "/dev/($OSNAME)_loop" ]; then
    export ($OSNAME)_loop=losetup --partscan --show --find "$BUILDDIR/$OSNAME.img" "/dev/($OSNAME)_loop" 
fi
