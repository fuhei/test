#!/bin/bash

set -e

gcc mmap.c -pg \
	-Wall -Werror \
	-flto=auto \
	-ffat-lto-objects \
	$* \
	-pthread \
	-lrt \
	-o mmap.out

