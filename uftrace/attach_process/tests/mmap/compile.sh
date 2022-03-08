#!/bin/bash

set -e

gcc mmap.c -pg -Wall -Werror \
	-pthread \
	-lrt \
	-o mmap.out

