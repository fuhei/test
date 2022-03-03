#!/bin/bash

set -e

gcc test.c -pg -o a.out
gcc -Wall -Werror attach.c elf_utils.c rbtree.c maps.c process.c -I. -o attach.out

clear

./attach.out

