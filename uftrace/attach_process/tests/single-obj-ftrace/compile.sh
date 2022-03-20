#!/bin/bash

set -e

make -C ../../build/

# main.out
gcc main.c -pg -g -ggdb\
	-Wall -Werror \
	-flto=auto \
	-ffat-lto-objects \
	$* \
	-pthread \
	-lrt \
	-o main.out

# Trampoline
#nasm -felf64 hello.asm
#nasm -felf64 exit.asm
gcc -c trampoline0-pure-func.c
gcc -c trampoline1-exit.c
gcc -c trampoline1-printf.c
gcc -c trampoline2-printf-var.c
