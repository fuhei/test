#!/bin/bash

set -e

# 整体编译过程
clang++ -O2 -g code.cc -o code
perf record -b ./code
create_llvm_prof --binary=./code --out=code.prof
clang++ -O2 -g -fprofile-sample-use=code.prof code.cc -o code
