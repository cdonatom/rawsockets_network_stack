#include "ripv2.h"
#include "../udp/udp.h"
#include "../ipv4/ipv4.h"

#include <stdio.h>
#include <stdlib.h>

int main ( int argc, char * argv[] ){
	if (argc != 2) {
		printf("Uso: %s <ipv4_destino> \n", argv[0]); 
		exit(-1);
	}
	ipv4_addr_t ipv4_addr;
	int err;
	char* filename_conf="ipv4/config/ipv4_config_client.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_client.txt";
	char* ipv4_addr_str = argv[1];
	err = ipv4_str_addr(ipv4_addr_str, ipv4_addr);
	if(err == -1){
		printf("La dirección IP introducida no es correcta.\n");
		exit(-1);
	}
	//Inicializamos UDP con el puerto de RIP:
	err = udp_open(filename_conf, filename_rutas, RIP_PORT);
	if (err == -1){
		printf("Ha habido un problema al abrir UDP.\n");
		exit(-1);
	}
	uint16_t socket_rcv;
	rip_packet_t rip_packet;
	//Creamos el paquete REQUEST:
	create_request( &rip_packet);
	//Lo mandamos a traves de UDP:
	err = udp_send( ipv4_addr, RIP_PORT, (void*)&rip_packet, REQUEST_SIZE);
	printf("Se han enviado %i bytes.\n", err);
#ifdef DEBUG
	print_rip_packet (&rip_packet, REQUEST_SIZE);
#endif
	//Comenzamos a escuchar durante 2 segundos para recibir un paquete RIP:
	err = udp_recv(&socket_rcv, ipv4_addr, (void*) &rip_packet, 2000);
#ifdef DEBUG
	if( err > 0 ){
		print_rip_packet ( &rip_packet, err );
	}	
#endif
	udp_close();
	return 0;
}