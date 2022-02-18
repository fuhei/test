#!/bin/bash

# 先生成汇编，再优化编译
gcc cal.c -O0 -S -o cal-O0.s
gcc cal-O0.s -O3 -o cal-O0s-O3.out
gcc cal-O0.s -o cal-O0s-notO3.out
# 通过分析，cal-O0s-O3.out 和 cal-O0s-notO3.out 二进制 diff 和 objdump 结果显现完全一样

# 直接优化编译
gcc cal.c -O3 -o cal-O3.out
