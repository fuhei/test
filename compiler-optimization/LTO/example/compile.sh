#!/bin/bash

# 编译 单文件
gcc single-src.c -o single.out

# 编译 多文件
gcc -c multi-srclib.c 
gcc -c multi-src.c 
gcc multi-src.o  multi-srclib.o -o multi.out

# 添加 -flto 选项
gcc multi-src.o  multi-srclib.o -flto -o multi-lto.out
