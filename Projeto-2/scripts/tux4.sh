#!/bin/bash
ifconfig eth0 down
ifconfig eth1 down
ifconfig eth0 up 172.16.30.254/24
ifconfig eth1 up 172.16.31.253/24
route add default gw 172.16.31.254
echo 1 > /proc/sys/net/ipv4/ip_forward
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
