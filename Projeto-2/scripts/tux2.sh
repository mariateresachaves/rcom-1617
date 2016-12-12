#!/bin/bash
ifconfig eth0 down
ifconfig eth0 up 172.16.31.1/24
route add default gw 172.16.31.253
route add -net 172.16.30.0/24 gw 172.16.31.253
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
