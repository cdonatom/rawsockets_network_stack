#include "udp.h"
#include "../ipv4/ipv4.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>

#define UDP_PAYLOAD_LEN (IPv4_PAYLOAD_LEN - UDP_HEADER_SIZE)

int main ( int argc, char * argv[] ){
	if (argc != 2) {
		printf("Uso: %s <socket_recv>\n", argv[0]); 
		exit(-1);
	}
	/* Variables */
	char* filename_conf="ipv4/config/ipv4_config_server.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_server.txt";
	int err;
	ipv4_addr_t ipv4_addr;
	uint16_t socket_rcv;
	socket_rcv = atoi ( argv[1] );
	char buffer[UDP_PAYLOAD_LEN];

	err = udp_open(filename_conf, filename_rutas, socket_rcv);
	if (err == -1){
		printf("Ha habido un problema al abrir UDP.\n");
		exit(-1);
	}
	while(1){
		memset(buffer, 0, UDP_PAYLOAD_LEN);
		err = udp_recv ( &socket_rcv, &ipv4_addr, buffer, -1);
		printf("%s", buffer);
		
		if( err > 0){
			printf("Payload length: %i\n", err);
			err = udp_send (ipv4_addr, socket_rcv, buffer, err );
			printf("Payload: %s\n", buffer);
			printf("Reenvio el paquete \n");
		}		
	}
}
