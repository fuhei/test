#!/bin/bash

# 构建过程指定宏
rpmbuild -ba rpmbuild/SPECS/openstack-cinder.spec --define '_sysconfdir /test'

# 如构建 grub2-src.rpm包
rpmbuild -ba grub2.spec --define 'efi_vendor ceaos'
