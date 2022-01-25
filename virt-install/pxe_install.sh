#!/bin/bash

# 这是个失败的指令
virt-install \
	--name $1 \
	--ram 8192 \
	--location http://192.168.122.22/CCLinux-2203-x86_64-rc1 \
	--file-size 10 \
	--graphics vnc,listen=0.0.0.0 \
	--pxe \
	--network default
