#!/bin/bash

VM_NAME=cclinux-2203-1
#ISO_PATH=/home/isos/CentOS-Stream-9-20220107.0-x86_64-dvd1.iso
ISO_PATH=/home/isos/CCLinux-2203-20220124.1-x86_64-dvd1.iso

virt-install --virt-type=kvm \
	--name $VM_NAME\
	--memory 8192 \
	--vcpus 8 \
	--os-type=linux \
	--os-variant=centos8 \
	--cdrom=$ISO_PATH \
	--network=bridge=br0,model=virtio \
	--graphics vnc,listen=0.0.0.0 \
	--disk path=/var/lib/libvirt/images/$VM_NAME.qcow2,size=100,format=qcow2 \
	--check all=off
