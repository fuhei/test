#!/bin/bash

# 创建 网桥
brctl addbr br0

# 配置 网桥 IP 地址
ip address add 192.168.0.1/24 dev br0

# 启动 网桥
ip link set dev br0 up

# 查看网桥的IP 地址
ip address show br0

# 添加 veth 设备 veth1 到 br0 中
brctl addif br0 veht0

# 显示信息
brctl show

# 从 veht0 设备发送数据
ping -I veht0 192.168.0.1

# 在新的命令行中 真挺 br0 中的数据，可以看到从 veht0 设备的 MAC 地址 发送来的数据包
tcpdump -i br0 -elnv

# 常用命令
brctl addbr br1 			#创建网桥
brctl addif br1 enp4s0 		#为网桥添加物理接口
brctl delbr br1 			#删除网桥
brctl delif br1 enp4s0 		#删除网桥接口
brctl show 					#显示网桥列表信息
brctl show br1 				#显示网桥br1的信息
brctl showmacs br1 			#显示网桥的MAC地址信息
brctl showstp br1			#显示网桥的stp信息
brctl stp br-test on|off	#开|关stp
