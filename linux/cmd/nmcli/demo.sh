

#markdown.nmcli connection 查询会话
#nmcli：网络管理命令字；connection ：连接，会话
nmcli connection delete con-mane #删除一个会话
#delete：删除 con_name：会话名字
#例：nmcli connection delete ens33

nmcli connection add type ethernet con-name ens33 ifname ens33 #添加一个会话
#type ethernet 网络类型是以太网 con_ens33：会话名字 网卡设备名：ifname：ens33

nmcli connection modify con-name iPv4.address ip/netmask #给会话添加ip地址
#例： nmcli connection modify ens33 ipv4.addresses 192.168.8.3/24

nmcli connection modify con-name ipv4.gateway ip #给会话添加网关
#例：nmcli connection modify ens33 ipv4.gateway 192.168.8.1

nmcli connection modify con-name ipv4.dns ip #给会话提阿尼啊dns服务器地址
例：nmcli connection modify ens33 ipv4.dns 144.144.144.144

nmcli connection modify ens33 ipv4.method manual #保存配置

nmcli connection up ens33 #激活会话

nmcli connection down ens33 #关闭会话编辑器
