#!/bin/bash

# 查看80端口占用情况
sudo netstat -ntulp | grep 80
# tcp6       0      0 :::80                   :::*                    LISTEN      943/httpd
