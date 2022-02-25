#!/bin/bash

gcc main.c -S -o main.0.s

# -flto=auto
gcc main.c -flto=auto -S -o main.1.s

# -ffat-lto-objects
gcc main.c -flto=auto -ffat-lto-objects -S -o main.2.s


echo "---------------------------------------------"

