#ifndef _ARP_H
#define _ARP_H

#include "../eth/eth.h"
#include "../ipv4/ipv4.h"
#include "arp_cache.h"

#include <stdio.h>

/* int arp_open();
 *
 * DESCRIPCIÓN:
 *   Esta función inicializa la tabla cache de arp.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si ha ocurrido algún error.
 *
 */
int arp_open();

/* int arp_close ();
 *
 * DESCRIPCIÓN:
 *   Esta función cierra la tabla cache y libera su memoria.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si ha ocurrido algún error.
 *
 */
int arp_close();

/* arp_cache_table_t* get_my_cache();
 *
 * DESCRIPCIÓN:
 *   Esta función devuelve la tabla cache de arp.
 *
 * VALOR DEVUELTO:
 *  Devuelve la tabla cache.
 * 
 */
arp_cache_table_t* get_my_cache();

/* int arp_resolve
 *	(eth_iface_t * iface, ipv4_addr_t ip_addr, mac_addr_t mac_addr);
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener la dirección MAC correspondiente a una direccion IPv4 dada.
 *	 Para ello debemos indicar nuestra interfaz de salida, la dirección ip y una dirección MAC 
 *	 donde guardaremos la obtenida.
 *	 La función comprueba primero si tiene esa dirección IPv4 guardada en la tabla cache. Si 
 *	 es así, devuelve directamente la MAC de esa entrada. Si no la tiene guardada, envia una
 *	 petición ARP para resolver la dirección. Si pasados dos segundos no obtiene la respuesta
 *	 que esperaba, vuelve a enviar la petición, esta vez esperando 3 segundos.
 *
 * PARÁMETROS:
 *	'iface': Manejador de la interfaz Ethernet por la que se quiere
 *		 enviar el paquete.
 *
 *  'ip_addr': estructura que almacena la dirección IP que se desea resolver.
 *
 *	'mac_addr': estructura donde se va a guardar la dirección MAC.
 *
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si se ha transmitido 2 veces la petición de REQUEST
 *   y no se ha obtenido respuesta o si se ha producido algún error durante el proceso.
 *
 */
int arp_resolve
(eth_iface_t * iface, ipv4_addr_t ip_addr, mac_addr_t mac_addr);

#endif /* _ARP_H */
