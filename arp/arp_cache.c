#include "arp_cache.h"
#include "../eth/eth.h"
#include "../ipv4/ipv4.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* arp_cache_table_t* create_cache_table(); 
 *
 * DESCRIPCI�N:
 *   Esta funci�n inicializa una tabla cache de 10 entradas.
 *
 * VALOR DEVUELTO:
 *  Devuelve un puntero de tipo arp_cache_table_t* con la tabla cache ya inicializada.
 * 
 */
arp_cache_table_t* create_cache_table(){
	arp_cache_table_t* table;
	table = (arp_cache_table_t*) malloc(sizeof(arp_cache_table_t));
	return table;
}

/* int free_table (arp_cache_table_t* arp_cache_table); 
 *
 * DESCRIPCI�N:
 *   Esta funci�n libera la memoria reservada de la tabla cache indicada.
 *
 * PAR�METROS:
 *	'arp_cache_table': puntero con la tabla cache que deseamos liberar.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si la memoria se ha liberado correctamente.
 * 
 * ERRORES:
 *   La funci�n devuelve -1 si la tabla est� vac�a.
 *
 */
int free_table (arp_cache_table_t* arp_cache_table){
	if (arp_cache_table != NULL){
		free (arp_cache_table);
		return 0;
	}
	return -1;
}

/* int arp_cache_table_add 
 *	(arp_cache_table_t* table, ipv4_addr_t ipv4_addr, mac_addr_t mac_addr, int index); 
 *
 * DESCRIPCI�N:
 *   Esta funci�n inserta en la tabla cache indicada una entrada con una direcci�n ip 
 *	y su MAC correspondiente.
 *
 * PAR�METROS:
 *	'table': puntero de tipo arp_cache_table_t* donde queremos realizar la entrada.
 *	'ipv4_addr': direcci�n ip que queremos introducir.
 *	'mac_addr': direcci�n MAC que queremos asociar a la anterior ip.
 *	'index': posicion de la tabla cache donde queremos meter la entrada.
 *
 * VALOR DEVUELTO:
 *  Devuelve la posici�n de la tabla donde se realiz� la insercion.
 * 
 * ERRORES:
 *   La funci�n devuelve -1 si la tabla est� vac�a o la posici�n indicada no es correcta.
 *
 */
int arp_cache_table_add 
(arp_cache_table_t* table, ipv4_addr_t ipv4_addr, mac_addr_t mac_addr, int index){
	if (table != NULL && (index >= 0 && index < ARP_CACHE_NUM_ENTRYS)){
		memcpy((void*)&(table->arp_entrys[index].ipv4_addr), ipv4_addr, IPv4_ADDR_SIZE);
		memcpy((void*)&(table->arp_entrys[index].mac_addr), mac_addr, MAC_ADDR_SIZE);
		return index;
	}
	return -1;
}