#!/bin/bash

# 重新打包已经安装的RPM包,
# 指定编辑器为 vim
EDITOR=vim rpmrebuild -e mozjs78
