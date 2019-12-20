#include "ipv4.h"
#include <stdio.h>
#include <stdlib.h>


int main ( int argc, char * argv[] ){
	
	/* Procesar argumentos */
	if (argc != 2) {
		printf("Uso: %s <ipv4_destino>\n", argv[0]); 
		exit(-1);
	}
	
	/* Variables */
	char* ip_addr_str= argv[1];
	char* filename_conf="ipv4/config/ipv4_config_client.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_client.txt";
	int err;
	char buffer [64];
	int i;
	
	ipv4_addr_t ipv4_addr_dest;
	
	err=ipv4_str_addr(ip_addr_str, ipv4_addr_dest );
	if(err == -1){
		printf("La dirección IP introducida no es correcta.\n");
		exit(-1);
	}
	
	for(i=0; i<64; i++) buffer[i]=0;
	
	err = ipv4_open(filename_conf, filename_rutas);
	
	err = ipv4_send(ipv4_addr_dest, 0x11, buffer, 64); //UDP

	err = ipv4_recv (0x11, ipv4_addr_dest, buffer, -1);
	ipv4_close();
	return 0;
}
