#!/bin/bash

# 从 语法分析树中打印 源玛
clang -cc1 t.c -ast-print
