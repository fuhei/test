#!/bin/bash

set -e

gcc mmap.c -pg -Wall -Werror -O3 \
	-pthread \
	-lrt \
	-o mmap.out

