#!/bin/bash

# 创建虚拟机存储qcow2文件
qemu-img create -f qcow2 /tmp/centos7.qcow2 10G
