#!/bin/bash

lsblk -l -p -o name,rm,ro,hotplug,size,type,mountpoint,uuid
# NAME                RM RO HOTPLUG   SIZE TYPE MOUNTPOINT UUID
# /dev/mapper/cl-root  0  0       0    70G lvm  /          1d472625-a7c5-4a0e-85c8-39221b1ac47e
# /dev/mapper/cl-swap  0  0       0   7.8G lvm  [SWAP]     e97bc9db-7e93-4acf-8b88-3189fc3f585b
# /dev/mapper/cl-home  0  0       0 386.4G lvm  /home      ee5b525c-c099-49a2-8ee7-95e63eaabd90
# /dev/nvme0n1         0  0       0 465.8G disk            
# /dev/nvme0n1p1       0  0       0   600M part /boot/efi  8099-2716
# /dev/nvme0n1p2       0  0       0     1G part /boot      a02fa707-d969-4b1a-bd74-5cdad54d91ab
# /dev/nvme0n1p3       0  0       0 464.2G part            s10Ntx-6RnS-ojIj-X92l-19J2-80uh-PZ3Ep8
