#!/bin/bash

gcc main.c -S -o main.0.s
# -ffat-lto-objects
gcc main.c -ffat-lto-objects -S -o main.1.s


echo "---------------------------------------------"

