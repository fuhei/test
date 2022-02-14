#!/bin/bash

# decompress
unsquashfs install.img
[ $? -eq 0 ] || (echo "unsquashfs failed"; exit)

cd squashfs-root/LiveOS/
mkdir mounting_here
mount rootfs.img mounting_here/
cd mounting_here/

# modify,add,del file of install.img
# ...

cd ..

# compress
umount mounting_here/
rmdir mounting_here/
cd ../../
mv install.img{,.backup}
mksquashfs squashfs-root/ install.img -comp xz -b 64k -Xdict-size 100%
echo "install.img is created successfully!"
