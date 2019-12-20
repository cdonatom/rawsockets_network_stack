#include "ripv2.h"
#include "ripv2_route_table.h"	
#include "../ipv4/ipv4.h"
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>

/* void create_request( rip_packet_t* rip_packet );
 * 
 * DESCRIPCIÓN: 
 *   Esta función inicializa un mensaje RIP de tipo Request poniendo todos los campos a 0.
 *
 * PARÁMETROS:
 *   'rip_packet': El paquete rip que queremos inicializar.
 *
 */
void create_request( rip_packet_t* rip_packet ){
	rip_entry_t entry;
	rip_packet->command= REQUEST;
	rip_packet->version = RIP_VERSION;
	rip_packet->zeros = 0;
	entry.afi = 0;
	entry.route_tag = 0;
	memset(entry.ipv4_addr, 0, IPv4_ADDR_SIZE);
	memset(entry.subnet_mask, 0, IPv4_ADDR_SIZE);
	memset(entry.next_hop, 0, IPv4_ADDR_SIZE);
	entry.metric = htonl(INFINITY_METRIC);
	memcpy((void*)&(rip_packet->list_entry[0]), (void*)&entry, sizeof(rip_entry_t));
}

/* int create_response(rip_packet_t* rip_packet, ripv2_route_table_t* rip_table);
 * 
 * DESCRIPCIÓN: 
 *   Esta función nos crea un paquete RIP de tipo RESPONSE. Obtiene de la tabla de rutas
 *	RIP los datos para crear las entradas de la lista que lleva el paquete. Añade todas
 *	las rutas.
 *
 * PARÁMETROS:
 *   'rip_packet': Paquete RIP de tipo RESPONSE que queremos crear.
 *		
 *   'rip_table': La tabla de rutas RIP de la que queremos obtener las entradas.
 *
 * VALOR DEVUELTO:
 *   Devolvemos el tamaño en bytes del paquete creado.
 * 
 */
int create_response(rip_packet_t* rip_packet, ripv2_route_table_t* rip_table){
	rip_packet->command= RESPONSE;
	rip_packet->version = RIP_VERSION;
	rip_packet->zeros = 0;
	int i, num_entrys;
	num_entrys = 0;
	rip_entry_t entry;
	ripv2_route_t* route;
	for(i = 0; i < RIPv2_ROUTE_TABLE_SIZE; i++){
		route = ripv2_route_table_get( rip_table, i );
		if(route != NULL){
			entry.afi = htons(2);
			entry.route_tag = 0;
			memcpy(entry.ipv4_addr, route->subnet_addr, IPv4_ADDR_SIZE);
			memcpy(entry.subnet_mask,route->subnet_mask, IPv4_ADDR_SIZE);
			memset(entry.next_hop, 0, IPv4_ADDR_SIZE);
			entry.metric = htonl(route->metric);
			memcpy((void*)&(rip_packet->list_entry[i]), (void*)&entry, sizeof(rip_entry_t));
			num_entrys++;
		}
	}
	return (RIP_PACKET_HEADER + num_entrys*sizeof(rip_entry_t));//Devolvemos el tamaño en bytes del paquete creado.
}

/* void print_rip_packet ( rip_packet_t* rip_packet, int packet_len);
 * 
 * DESCRIPCIÓN: 
 *   Esta función imprime por pantalla el paquete RIP indicado con todas las entradas que lleva.
 *
 * PARÁMETROS:
 *   'rip_packet': Paquete RIP que queremos mostrar.
 *		
 *   'packet_len': Longitud del paquete RIP dado.
 * 
 */
#ifdef DEBUG
void print_rip_packet ( rip_packet_t* rip_packet, int packet_len){
	int i;
	int limit = (packet_len - RIP_PACKET_HEADER)/sizeof(rip_entry_t); //Restamos 4 bytes de command, version y zeros y dividimos entre el tamaño de la estructura.
	char ipv4_str[IPv4_ADDR_STR_LENGTH];
	printf("Paquete RIP\n\n");
	printf( "RIP command: %i\n", rip_packet->command );
	printf( "RIP version: %i\n", rip_packet->version );
	for(i = 0; i < limit; i++){
		printf("\nEntry Number: %i\n", i);
		printf("Address Family Identifier: %i\n", ntohs(rip_packet->list_entry[i].afi));
		printf("Route Tag: %i\n", ntohs(rip_packet->list_entry[i].route_tag));
		ipv4_addr_str ( rip_packet->list_entry[i].ipv4_addr, ipv4_str );
		printf("IPv4 Address: %s\n", ipv4_str);
		ipv4_addr_str ( rip_packet->list_entry[i].subnet_mask, ipv4_str );
		printf("Subnet Mask: %s\n", ipv4_str);
		ipv4_addr_str ( rip_packet->list_entry[i].next_hop, ipv4_str );
		printf("Next Hop: %s\n", ipv4_str);
		printf("Metric: %i\n", ntohl(rip_packet->list_entry[i].metric));
	}
}
#endif