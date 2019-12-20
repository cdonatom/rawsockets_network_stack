#! /bin/bash

if [ $# = 1 ]; then
	
	T1="-arp"
	T2="-arp_d"
	T3="-eth"
	T4="-ipv4"
	T5="-ipv4_d"
	T6="-udp"
	T7="-udp_d"
	T8="-rip"
	T9="-rip_d"
	
	if [ "$1" = "$T1" ]; then 
		echo Compilando cliente y servidor ARP
		echo ....
		echo ..
		rawnetcc /tmp/arp_client arp/arp_client.c arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_route_table.c ipv4/ipv4_config.c -static
		rawnetcc /tmp/arp_server arp/arp_server.c arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_route_table.c ipv4/ipv4_config.c -static 
		exit 0
	fi
	if [ "$1" = "$T2" ]; then 
		echo Compilando cliente y servidor ARP Debug
		echo ....
		echo ..
		rawnetcc /tmp/arp_client arp/arp_client.c arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_route_table.c ipv4/ipv4_config.c -static  -DDEBUG
		rawnetcc /tmp/arp_server arp/arp_server.c arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_route_table.c ipv4/ipv4_config.c -static  -DDEBUG
		exit 0
	fi
	if [ "$1" = "$T3" ]; then
		echo Compilando cliente y servidor Ethernet
		echo ....
		echo ..
		rawnetcc /tmp/eth_client eth/eth_client.c eth/eth.c eth/debug.c -static 
		rawnetcc /tmp/eth_server eth/eth_server.c eth/eth.c eth/debug.c -static 
		exit 0
	fi
	if [ "$1" = "$T4" ]; then 
		echo Compilando cliente y servidor IPv4
		echo ....
		echo ..
		rawnetcc /tmp/ipv4_client arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c ipv4/ipv4_client.c -static 
		rawnetcc /tmp/ipv4_server arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c ipv4/ipv4_server.c -static 
		exit 0
	fi
	if [ "$1" = "$T5" ]; then 
		echo Compilando cliente y servidor IPv4 debug
		echo ....
		echo ..
		rawnetcc /tmp/ipv4_client arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c ipv4/ipv4_client.c -static  -DDEBUG
		rawnetcc /tmp/ipv4_server arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c ipv4/ipv4_server.c -static  -DDEBUG
		exit 0
	fi
	if [ "$1" = "$T6" ]; then 
		echo Compilando cliente y servidor UDP
		echo ....
		echo ..
		rawnetcc /tmp/udp_client arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c udp/udp_client.c -static 
		rawnetcc /tmp/udp_server arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c udp/udp_server.c -static 
		exit 0
	fi
	if [ "$1" = "$T7" ]; then 
		echo Compilando cliente y servidor UDP debug
		echo ....
		echo ..
		rawnetcc /tmp/udp_client arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c udp/udp_client.c -static -DDEBUG
		rawnetcc /tmp/udp_server arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c udp/udp_server.c -static -DDEBUG
		exit 0
	fi
	if [ "$1" = "$T8" ]; then 
		echo Compilando cliente y servidor RIP
		echo ....
		echo ..
		rawnetcc /tmp/rip_client arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c rip/ripv2.c rip/ripv2_route_table.c rip/ripv2_client.c -static 
		rawnetcc /tmp/rip_server arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c rip/ripv2.c rip/ripv2_route_table.c rip/ripv2_server.c -static 
		exit 0
	fi
	if [ "$1" = "$T9" ]; then 
		echo Compilando cliente y servidor RIP debug
		echo ....
		echo ..
		rawnetcc /tmp/rip_client arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c rip/ripv2.c rip/ripv2_route_table.c rip/ripv2_client.c -static  -DDEBUG
		rawnetcc /tmp/rip_server arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c rip/ripv2.c rip/ripv2_route_table.c rip/ripv2_server.c -static  -DDEBUG
		rawnetcc /tmp/rip_fake_router arp/arp_cache.c arp/arp.c eth/eth.c eth/debug.c ipv4/ipv4.c ipv4/ipv4_config.c ipv4/ipv4_route_table.c udp/udp.c rip/ripv2.c rip/ripv2_route_table.c rip/fake_router_rip.c -static  -DDEBUG
		exit 0
	fi
	
else 
	echo Empleo de uso \"sh compila.sh -[arp] -[arp_d] -[eth] -[ipv4] -[ipv4_d] -[udp] -[udp_d]\"
	exit 0
fi