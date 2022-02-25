#!/bin/bash

gcc main.c -S -o main.s
gcc -fcf-protection main.c -S -o main-fcf-protection.s
