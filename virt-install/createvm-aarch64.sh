#!/bin/bash

# 配置NAT网络
# --network default

virt-install --virt-type=kvm \
	--name centos-stream-9.2 \
	--memory 8192 \
	--vcpus 8 \
	--os-type=linux \
	--os-variant=centos8 \
	--cdrom=/home/isos/CentOS-Stream-9-20220107.0-aarch64-dvd1.iso \
	--network=bridge=br0,model=virtio \
	--graphics vnc,listen=0.0.0.0 \
	--disk path=/var/lib/libvirt/images/centos-stream-9-2.qcow2,size=100,format=qcow2 \
	--check all=off
