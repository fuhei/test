#!/bin/bash

SRC_DIR=CentOS-Stream-9-20220107.0-aarch64-dvd1
DST_ISO=CCLinux-2203-aarch64-dvd1.iso
LABEL=CCLinux-2203-aarch64
#LABEL=CentOS-Stream-9-BaseOS-aarch64

genisoimage -r -J -T -v \
	-V $LABEL \
	-c boot.catalog \
	-hide boot.catalog \
	-e images/efiboot.img \
	-no-emul-boot \
	-o $DST_ISO \
	$SRC_DIR

implantisomd5 ./$DST_ISO

echo > $DST_ISO.manifest
for f in `find $SRC_DIR`
do 
	echo ${f:${#SRC_DIR}} >> $DST_ISO.manifest
done
