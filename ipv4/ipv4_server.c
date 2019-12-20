#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>

#define TIMEOUT -1

int main (){

	unsigned char buffer[IPv4_PAYLOAD_LEN];
	ipv4_addr_t ipv4_src;
	char* filename_conf="ipv4/config/ipv4_config_server.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_server.txt";
	int err;

	ipv4_open(filename_conf, filename_rutas);

	while(1){
		err = ipv4_recv (0x99, ipv4_src, buffer, -1);
		
		if( err > 0){
			err = ipv4_send (ipv4_src, 0x99, buffer, err); 
		}
				
	}
	ipv4_close();
}
