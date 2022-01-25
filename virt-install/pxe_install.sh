#!/bin/bash

# 这是个失败的指令
virt-install \
	--name $1 \
	--ram 8192 \
	--file-size 10 \
	--graphics vnc,listen=0.0.0.0 \
	--pxe \
	--network default
