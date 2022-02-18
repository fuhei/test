#!/bin/bash

# ref: https://www.server-world.info/en/note?os=CentOS_Stream_9&p=initial_conf&f=3
# disable IPv6
grubby --update-kernel ALL --args ipv6.disable=1

# reboot and show
reboot
ip address show


# if falling back to enable, run like follows
grubby --update-kernel ALL --remove-args ipv6.disable
