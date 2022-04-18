#!/bin/bash

# 生成 LLVM
clang t.c $* -S -emit-llvm -o -
