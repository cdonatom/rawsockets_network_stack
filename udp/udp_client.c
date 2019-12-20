#include "udp.h"
#include "../ipv4/ipv4.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define UDP_PAYLOAD_LEN (IPv4_PAYLOAD_LEN - UDP_HEADER_SIZE)


int main ( int argc, char * argv[] ){
	if (argc != 5) {
		printf("Uso: %s <ipv4_destino> <socket_send> <socket_rcv> <mensaje>\n", argv[0]); 
		exit(-1);
	}
	int err;
	char* ipv4_addr_str = argv[1];
	char* filename_conf="ipv4/config/ipv4_config_client.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_client.txt";
	ipv4_addr_t ipv4_addr;
	uint16_t socket_send, socket_rcv;
	char buffer[UDP_PAYLOAD_LEN];
	
	socket_send = atoi( argv[2] );
	socket_rcv = atoi ( argv[3] );
	err = ipv4_str_addr(ipv4_addr_str, ipv4_addr) ;
	if(err == -1){
		printf("La dirección IP introducida no es correcta.\n");
		exit(-1);
	}
	char* payload= argv[4];
	err = udp_open(filename_conf, filename_rutas, socket_rcv);
	if (err == -1){
		printf("Ha habido un problema al abrir UDP.\n");
		exit(-1);
	}
	
	err = udp_send (ipv4_addr, socket_send, (void*) payload, (int) strlen(payload));
	printf("Payload length: %i\n", (int) strlen(payload));
	if ( err == -1){
		udp_close();
		exit(-1);
	}
	err = udp_recv ( &socket_rcv, &ipv4_addr, buffer, 2000 );
	if ( err == -1){
		udp_close();
		exit(-1);
	}
	printf("He recibido con exito: %s \n", buffer);
	udp_close();
	return 0;
}
