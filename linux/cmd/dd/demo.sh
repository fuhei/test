#!/bin/bash

# 执行 50w 次 1KB 的传输
dd if=/dev/zero of=/dev/null bs=1k count=500k

# 读写测试
dd if=/dev/zero of=file1.out bs=1024k count=1k
dd if=file1.out of=/dev/null bs=1024k

# skip
dd if=file bs=1024 count=60 skip=0  of=file1
dd if=file bs=1024 count=60 skip=60 of=file2

# 热补丁示例
# 跳过upatch头
dd if=foo.upatch bs=1 count=10336 skip=760 of=a.orig
dd if=tup.foo.upatch.gZMZ7U bs=1 count=10336 skip=760 of=a.patched

