#!/bin/bash

set -e

trap "pkill -9 mmap.out" SIGINT

cd ../build/
cmake ..
make
cd -


if [ $# -lt 1 ]; then
	echo ">>>>> Attach PID $(pidof mmap.out) <<<<<"
	mmap/mmap.out &
	../build/upatch -p $(pidof mmap.out)
else
	./build/upatch $*
fi

