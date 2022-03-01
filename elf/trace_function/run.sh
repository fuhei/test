#!/bin/bash

gcc check_trace_function.c -o check.out
gcc -pg main-loop.c -o a.out

./check.out

rm -f *.out
