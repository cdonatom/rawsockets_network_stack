#ifndef _RIPv2_H
#define _RIPv2_H 

#include "ripv2_route_table.h"	
#include "../ipv4/ipv4.h"

#include <stdint.h>

#define RIP_PORT 520
#define REQUEST 1
#define RESPONSE 2
#define RIP_VERSION 2
#define ENTRY_MAX 25
#define INFINITY_METRIC 16
#define REQUEST_SIZE 24
#define RIP_PACKET_HEADER 4

/*Estructura de una entrada RIP*/
struct rip_entry{
	uint16_t afi; //Address Family Identifier.
	uint16_t route_tag;
	ipv4_addr_t ipv4_addr;
	ipv4_addr_t subnet_mask;
	ipv4_addr_t next_hop;
	uint32_t metric;
};

typedef struct rip_entry rip_entry_t;

/*Estructura del paquete RIP*/
struct rip_packet {
	uint8_t command; //Request = 1, Response = 2.
	uint8_t version;
	uint16_t zeros;
	rip_entry_t list_entry[ENTRY_MAX];
};

typedef struct rip_packet rip_packet_t;

/* void create_request( rip_packet_t* rip_packet );
 * 
 * DESCRIPCIÓN: 
 *   Esta función inicializa un mensaje RIP de tipo Request poniendo todos los campos a 0.
 *
 * PARÁMETROS:
 *   'rip_packet': El paquete rip que queremos inicializar.
 *
 */
void create_request( rip_packet_t* rip_packet );

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
int create_response(rip_packet_t* rip_packet, ripv2_route_table_t* rip_table);

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
void print_rip_packet ( rip_packet_t* rip_packet, int packet_len);
#endif

#endif
