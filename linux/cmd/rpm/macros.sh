#!/bin/bash

# 查看宏定义
rpm --eval "%{efi_vendor}"

# 查看所有宏定义并过滤
rpm --showrc  | grep efi_vendor

# 构建过程指定宏
rpmbuild -ba rpmbuild/SPECS/openstack-cinder.spec --define '_sysconfdir /test'
