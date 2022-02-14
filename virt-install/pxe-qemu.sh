#!/bin/bash
# Rong Tao 
# 2022-02-11
# 安装过程可以用tcpdump抓包
# tcpdump -i enp0s8 port 67 or port 68 or port 69 or port 80 -n -e -vv

VM_NAME=os-test

NETWORK=bridge:pxebr0

sudo virt-install \
		--virt-type=kvm \
		--name $VM_NAME\
		--memory 8192 \
		--vcpus 8 \
		--os-type=linux \
		--os-variant=centos-stream9 \
		--pxe \
		--network=$NETWORK,model=virtio \
		--disk path=/var/lib/libvirt/images/$VM_NAME.qcow2,size=10,format=qcow2 \
		--check all=off
