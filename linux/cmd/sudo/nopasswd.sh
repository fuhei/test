#!/bin/bash

# 允许wheel组内的用户无密码执行sudo
sed -i 's/^%wheel.*/%wheel  ALL=(ALL)       NOPASSWD: ALL/g' /etc/sudoers
