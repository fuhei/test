#!/bin/bash

SRC_DIR=CentOS-Stream-9-20220107.0-x86_64-dvd1
DST_ISO=CCLinux-2203-x86_64-dvd1.iso
LABEL=CCLinux-2203-x86_64

genisoimage -r -J -T -v \
	-V $LABEL \
	-b isolinux/isolinux.bin \
	-c isolinux/boot.cat \
	-no-emul-boot \
	-boot-load-size 4 \
	-boot-info-table -eltorito-alt-boot \
	-e images/efiboot.img \
	-no-emul-boot \
	-o  ./$DST_ISO \
	./$SRC_DIR

implantisomd5 ./$DST_ISO

echo > $DST_ISO.manifest
for f in `find $SRC_DIR`
do 
	echo ${f:${#SRC_DIR}} >> $DST_ISO.manifest
done
