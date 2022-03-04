#!/bin/bash

set -e

gcc test.c -pg  -o a.out
gcc -Wall -Werror \
	attach.c \
	elf_utils.c \
	file_utils.c \
	rbtree.c \
	maps.c \
	process.c \
	-I. \
	-o attach.out

trap "pkill -9 a.out" SIGINT

clear
./a.out &
#sleep 1
./attach.out -p $(pidof a.out)

