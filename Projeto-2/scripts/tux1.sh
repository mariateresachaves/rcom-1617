#!/bin/bash
ifconfig eth0 down
ifconfig eth0 up 172.16.30.1/24
route add default gw 172.16.30.254
route add -net 172.16.31.0/24 gw 172.16.30.254
echo 0 > /proc/sys/net/ipv4/icmp_echo_ignore_broadcasts
