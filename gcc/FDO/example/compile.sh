#!/bin/bash

gcc sort.c -o sort.out
gcc -O3 sort.c -o sort_optimized-O3.out
gcc sort.c -o sort_instrumented.out -fprofile-generate
./sort_instrumented.out 
gcc -O3 sort.c -o sort_fdo.out -fprofile-use=sort.gcda
