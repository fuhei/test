#!/bin/bash

fatal_1()
{
# 这是个失败的指令
virt-install \
	--name $1 \
	--ram 8192 \
	--file-size 10 \
	--graphics vnc,listen=0.0.0.0 \
	--pxe \
	--network default
}
fatal_2()
{
sudo virt-install --name=guest-name --ram=2048 --vcpus=1 \
	--disk path=/var/lib/libvirt/images/guest-disk.qcow2,size=10,bus=virtio \
	--pxe --graphics=vnc --hvm \
	--network network=default,model=virtio \
	--os-variant=centos-stream9
}

success_BIOS_1()
{
OS_NAME=os-1
virt-install --virt-type=kvm \
	--name $OS_NAME\
	--memory 8192 \
	--vcpus 8 \
	--os-type=linux \
	--os-variant=centos-stream9 \
	--pxe \
	--network=default,model=virtio \
	--disk path=/var/lib/libvirt/images/$VM_NAME.qcow2,size=10,format=qcow2 \
	--check all=off
}

# ERROR    Libvirt version does not support UEFI.
success_UEFI_1()
{
OS_NAME=os-1
virt-install --virt-type=kvm \
	--name $OS_NAME\
	--memory 8192 \
	--vcpus 8 \
	--os-type=linux \
	--os-variant=centos-stream9 \
	--pxe \
	--boot=uefi \
	--network=default,model=virtio \
	--disk path=/var/lib/libvirt/images/$VM_NAME.qcow2,size=10,format=qcow2 \
	--check all=off
}
