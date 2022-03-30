#!/bin/bash

# 查看宏定义
rpm --eval "%{efi_vendor}"

# 查看所有宏定义并过滤
rpm --showrc  | grep efi_vendor
