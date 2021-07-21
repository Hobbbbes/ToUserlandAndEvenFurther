#!/usr/bin/bash

BUILDDIR=$1
OSNAME=$2

if ! [ -f "$BUILDDIR/$OSNAME.img" ]; then
    dd if=/dev/zero of=$BUILDDIR/$OSNAME.img bs=4096 count=100000
    sgdisk --clear --new 1:2048:98304 --new 2:100352:799933 -t 1:EF00 -t 2:8300 $BUILDDIR/$OSNAME.img
fi
LOOP=`cat loopdevice`
IMAGE_PATH=`losetup --list --noheadings -O BACK-FILE ${LOOP}`
if ! [ -f "${IMAGE_PATH}" ]; then 
    losetup --partscan --show --find "$BUILDDIR/$OSNAME.img" > loopdevice
    partprobe `cat loopdevice` || true > /dev/null
fi