#!/bin/bash

# ref: https://www.jianshu.com/p/ce1b5a2437a9
#检查操作系统是否支持 cgroups v2
grep cgroup2 /proc/filesystems

#启用 cgroups v2 内核参数
#systemd.unified_cgroup_hierarchy=1

# 回到 cgroups v1
sudo grubby --update-kernel=ALL --args="systemd.unified_cgroup_hierarchy=0"
