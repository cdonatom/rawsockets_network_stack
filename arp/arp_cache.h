#ifndef _ARP_CACHE_H
#define _ARP_CACHE_H

#include <stdio.h>
#include "../eth/eth.h"
#include "../ipv4/ipv4.h"

#define ARP_CACHE_NUM_ENTRYS 10

//Estructura de una entrada de la tabla cache:
struct arp_cache_entry{
	ipv4_addr_t ipv4_addr;
	mac_addr_t mac_addr;
};

typedef struct arp_cache_entry arp_cache_entry_t;

//Estructura de la tabla cache:
struct arp_cache_table{
	arp_cache_entry_t arp_entrys[ARP_CACHE_NUM_ENTRYS];
};

typedef struct arp_cache_table arp_cache_table_t;

/* arp_cache_table_t* create_cache_table(); 
 *
 * DESCRIPCIÓN:
 *   Esta función inicializa una tabla cache de 10 entradas.
 *
 * VALOR DEVUELTO:
 *  Devuelve un puntero de tipo arp_cache_table_t* con la tabla cache ya inicializada.
 * 
 */
arp_cache_table_t* create_cache_table();

/* int free_table (arp_cache_table_t* arp_cache_table); 
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada de la tabla cache indicada.
 *
 * PARÁMETROS:
 *	'arp_cache_table': puntero con la tabla cache que deseamos liberar.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si la memoria se ha liberado correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si la tabla está vacía.
 *
 */
int free_table (arp_cache_table_t* arp_cache_table);

/* int arp_cache_table_add 
 *	(arp_cache_table_t* table, ipv4_addr_t ipv4_addr, mac_addr_t mac_addr, int index); 
 *
 * DESCRIPCIÓN:
 *   Esta función inserta en la tabla cache indicada una entrada con una dirección ip 
 *	y su MAC correspondiente.
 *
 * PARÁMETROS:
 *	'table': puntero de tipo arp_cache_table_t* donde queremos realizar la entrada.
 *	'ipv4_addr': dirección ip que queremos introducir.
 *	'mac_addr': dirección MAC que queremos asociar a la anterior ip.
 *	'index': posicion de la tabla cache donde queremos meter la entrada.
 *
 * VALOR DEVUELTO:
 *  Devuelve la posición de la tabla donde se realizó la insercion.
 * 
 * ERRORES:
 *   La función devuelve -1 si la tabla está vacía o la posición indicada no es correcta.
 *
 */
int arp_cache_table_add 
(arp_cache_table_t* table, ipv4_addr_t ipv4_addr, mac_addr_t mac_addr, int index);

#endif