#!/bin/bash

# CentOS/RHEL/Oracle/Scientific 和 Fedora Linux
sudo grub2-mkconfig -o /boot/grub2/grub.cfg
sudo grubby --set-default /boot/vmlinuz-5.6.9
# You can confirm the details with the following commands:
sudo grubby --info=ALL | more
sudo grubby --default-index
sudo grubby --default-kernel

# Debian/Ubuntu Linux
sudo update-initramfs -c -k 5.6.9
sudo update-grub
