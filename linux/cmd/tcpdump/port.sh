#!/bin/bash

# 过滤端口
tcpdump -i enp0s8 port 67 or port 68 or port 69 or port 80 -n -e -vv
