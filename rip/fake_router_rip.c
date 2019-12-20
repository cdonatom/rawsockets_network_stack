#include "ripv2.h"
#include "../udp/udp.h"
#include "../ipv4/ipv4.h"
#include "../ipv4/ipv4_route_table.h"
#include "ripv2_route_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NO_TIMEOUT -1

int main ( int argc, char * argv[] ){
	if (argc != 2) {
		printf("Uso: %s <ipv4_destino> \n", argv[0]); 
		exit(-1);
	}
	ipv4_addr_t ipv4_addr;
	int err;
	char* filename_conf="ipv4/config/ipv4_config_fake_router.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_fake_router.txt";
	char* ipv4_addr_str = argv[1];
	err = ipv4_str_addr(ipv4_addr_str, ipv4_addr);
	if(err == -1){
		printf("La dirección IP introducida no es correcta.\n");
		exit(-1);
	}
	err = udp_open(filename_conf, filename_rutas, RIP_PORT);
	if (err == -1){
		printf("Ha habido un problema al abrir UDP.\n");
		exit(-1);
	}
	//Tablas de rutas de IPv4.
	ipv4_route_table_t * ipv4_table = get_my_table();
	ipv4_route_t* ipv4_route;
	//Tablas de rutas de RIPv2.
	ripv2_route_table_t* rip_table = ripv2_route_table_create();
	ripv2_route_t * rip_route_ptr;
	int i=0;
	//Convertimos la tabla de rutas de IPv4 a RIPv2.
	for (i = 0; i < IPv4_ROUTE_TABLE_SIZE; i++){
		ipv4_route = ipv4_route_table_get ( ipv4_table, i );
		if ( ipv4_route != NULL){
			rip_route_ptr = ripv2_route_create (ipv4_route->subnet_addr,
				ipv4_route->subnet_mask, ipv4_route->iface, EMPTY_IPv4, 1, NO_TIMEOUT);//Metrica 1
			err = ripv2_route_table_add (rip_table, rip_route_ptr);
			if (err == -1){
				printf("Ha habido un error al insertar las rutas IPv4 en la tabla RIP.\n");
			}
		}
	}
	rip_packet_t rip_packet;
	memset ((void*) &rip_packet, 0, sizeof(rip_packet_t));
	
	int packet_size = create_response ( &rip_packet, rip_table);
	err = udp_send (ipv4_addr, RIP_PORT, (void*)&rip_packet, packet_size);
	
	ripv2_route_table_free(rip_table);
	udp_close();
	return 0;
}