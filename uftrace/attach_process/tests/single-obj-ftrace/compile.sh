#!/bin/bash

set -e

gcc -c trampoline.c -o trampoline.o

gcc main.c -pg \
	-Wall -Werror \
	-flto=auto \
	-ffat-lto-objects \
	$* \
	-pthread \
	-lrt \
	-o main.out

