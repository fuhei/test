#!/bin/bash

set -e

gcc main.c -pg -o a.out
gcc attach.c -o attach.out

