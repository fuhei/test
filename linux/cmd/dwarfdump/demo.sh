#!/bin/bash

gcc hello.c -g
dwarfdump a.out | more
