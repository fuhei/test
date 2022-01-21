#!/bin/bash

log_err()
{
echo -e "\033[1;31m$1\033[m"
}

log_err "Original."
./sort.out

log_err "Optimized with -O3"
./sort_optimized-O3.out

log_err "FDO"
./sort_fdo.out
