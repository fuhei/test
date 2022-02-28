#!/bin/bash

gcc main.c -o main.out

# -flto=auto
gcc --verbose main.c -flto=auto -o main.1.flto.out

# -ffat-lto-objects
gcc main.c -flto=auto -ffat-lto-objects -o main.2.flto.ffat-lto-objects.out


echo "---------------------------------------------"

