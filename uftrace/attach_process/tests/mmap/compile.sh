#!/bin/bash

set -e

gcc mmap.c -pg \
	-pthread \
	-lrt \
	-o mmap.out

