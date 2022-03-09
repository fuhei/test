#!/bin/bash

set -e

trap "pkill -9 main.out" SIGINT

cd ../../build/
cmake ..
make
cd -

if [ $# -lt 1 ]; then
	echo ">>>>> Attach PID $(pidof main.out) <<<<<"
	./main.out &
	sleep 1
	../../build/pattach -p $(pidof main.out) -f func2 --log-level 7 --ftrace-object ./trampoline.o
else
	../../build/pattach $*
fi

