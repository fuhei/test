#!/bin/bash

set -e

gcc uftrace.c -o uftrace.out
gcc main.c -pg -o a.out

