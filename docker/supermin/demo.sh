#!/bin/bash 
# 制作 docker 镜像
# ref: https://blog.csdn.net/hjh00/article/details/72859287

supermin -v --prepare bash coreutils -o supermin.d
supermin -v --build --format chroot supermin.d -o appliance.d
echo 8.4 > appliance.d/etc/yum/vars/releasever
tar --numeric-owner -cpf centos-8-2.tar -C appliance.d .

# import
cat centos-8.4.tar | docker import - centos:v8.4


