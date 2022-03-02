#!/bin/bash

# 过滤 openat(2) 系统调用
strace -eopenat /usr/bin/echo 'hello'

# 过滤 bpf(2) 系统调用
sudo strace -ebpf ./slabratetop
