#!/bin/bash

gcc_test()
{
gcc check_trace_function.c -o check.out
gcc -pg main-loop.c -o a.out

./check.out
}

clang_test()
{
clang check_trace_function.c -o check.out
clang -pg main-loop.c -o a.out

./check.out
}

gcc_test
clang_test

rm -f *.out
