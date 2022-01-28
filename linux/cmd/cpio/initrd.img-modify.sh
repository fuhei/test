#!/bin/bash

# decompress
mv initrd.img initrd.img.xz
mkdir -p temp
xz -d initrd.img.xz
mv initrd.img temp/
cd temp/
cat initrd.img | cpio -di
rm -f initrd.img

# modify,add,delete files in initrd.img
# ...

# compress
find . 2>/dev/null | cpio -c -o | xz -9 --format=lzma > ../initrd.img

