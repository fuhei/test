#!/bin/bash

set -e

gcc test.c -pg \
	-pthread \
	-lrt \
	-o a.out
gcc -Wall -Werror \
	attach.c \
	syscall.c \
	elf_utils.c \
	file_utils.c \
	rbtree.c \
	maps.c \
	process.c \
	-I. \
	-o attach.out

trap "pkill -9 a.out" SIGINT

clear

if [ $# -lt 1 ]; then
	./a.out &
	./attach.out -p $(pidof a.out)
else
	./attach.out $*
fi

