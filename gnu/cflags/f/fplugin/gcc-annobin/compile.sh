#!/bin/bash

# /usr/lib/rpm/redhat/redhat-annobin-cc1 文件内容
# 1 *cc1_options:
# 2 + %{!-fno-use-annobin:%{!iplugindir*:%:find-plugindir()} -fplugin=gcc-annobin}

# `man gcc`:
#        -specs=file
#          Process file after the compiler reads in the standard specs file, in order to override the defaults which the gcc
#          driver program uses when determining what switches to pass to cc1, cc1plus, as, ld, etc.  More than one -specs=file
#          can be specified on the command line, and they are processed in order, from left to right.

gcc main.c -S -o main.0.s

gcc main.c -S \
	-fplugin=gcc-annobin \
	-o main.2.s

gcc main.c -S \
	-specs=/usr/lib/rpm/redhat/redhat-annobin-cc1 \
	-o main.1.s



echo "---------------------------------------------"

