#!/bin/bash

set -e

gcc -fPIC -shared mcount-nop.c -o libmcount-nop.so
gcc main.c -pg -o a.out

