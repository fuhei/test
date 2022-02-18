#!/bin/bash

# ref: https://www.server-world.info/en/note?os=CentOS_Stream_9&p=initial_conf&f=3
# To set static IP address to the server, Modify settings like follows.
# ( Replace the interface name [enp1s0] to your own one because it's not the same on any System )

# if you did not set HostName, set it like follows
hostnamectl set-hostname dlp.srv.world

# display devices
nmcli device
#DEVICE  TYPE      STATE      CONNECTION
#enp1s0  ethernet  connected  enp1s0
#lo      loopback  unmanaged  --

# set IPv4 address
nmcli connection modify enp1s0 ipv4.addresses 10.0.0.30/24
# set gateway
nmcli connection modify enp1s0 ipv4.gateway 10.0.0.1

# set DNS
# for multiple DNS, specify with space separated ⇒ ipv4.dns "10.0.0.10 10.0.0.11 10.0.0.12"
nmcli connection modify enp1s0 ipv4.dns 10.0.0.10
# set DNS search base (your domain name -for multiple one, specify with space separated)
nmcli connection modify enp1s0 ipv4.dns-search srv.world
# set [manual] for static setting (it's [auto] for DHCP)
nmcli connection modify enp1s0 ipv4.method manual
# restart the interface to reload settings
nmcli connection down enp1s0; nmcli connection up enp1s0

# confirm settings
nmcli device show enp1s0

# confirm state
ip address show
