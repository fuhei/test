#!/bin/bash 
# https://yulistic.gitlab.io/2018/12/debugging-linux-kernel-with-gdb-and-qemu/
# 
# 生成 qcow2 文件
qemu-img create -f qcow2 test.qcow2 10G

# 安装 ISO 
qemu-system-x86_64 -m 2048 -enable-kvm -drive if=virtio,file=test.qcow2,cache=none -cdrom ../CentOS-8.5.2111-x86_64-dvd1.iso

# 运行一个虚拟机
qemu-system-x86_64 -m 2048 -chardev stdio,id=gdb0 -drive if=virtio,file=test.qcow2,cache=none -device isa-debugcon,iobase=0x402,chardev=gdb0,id=d1 -vga virtio -enable-kvm -vnc :1

# 运行一个虚拟机(GDB 调试)
# see https://yulistic.gitlab.io/2018/12/debugging-linux-kernel-with-gdb-and-qemu/
qemu-system-x86_64 -s -S -m 2048 -chardev stdio,id=gdb0 -drive if=virtio,file=test.qcow2,cache=none -device isa-debugcon,iobase=0x402,chardev=gdb0,id=d1 -vga virtio -enable-kvm -vnc :1

