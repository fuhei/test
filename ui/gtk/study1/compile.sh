#!/bin/bash

if [ $# -lt 1 ]; then
    echo "usage: $0 [main.c]"
    exit
fi

gcc $1 `pkg-config --libs --cflags gtk+-3.0`
