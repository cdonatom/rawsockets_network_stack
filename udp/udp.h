#ifndef _UDP_H
#define _UDP_H 

#include "../ipv4/ipv4.h"
#include <stdint.h>

#define UDP_HEADER_SIZE 8

/* int udp_open( char* filename_conf, char* filename_rutas, uint16_t socket_recv);
 *
 * DESCRIPCIÓN: 
 *   Esta función asigna el socket indicado por parámetros e incializa todo lo necesario
 *	de IP para que éste funcione.
 *
 * PARÁMETROS:
 *   'filename_conf': Nombre del fichero donde se encuentra nuestra configuración,
 *					es decir, nuestra interfaz, dirección IP y máscara de subred.
 *
 *  'filename_routas': Nombre del fichero donde se encuentran las rutas IP que se van a añadir
 *					a la tabla de rutas IP.
 *
 *	'socket_recv': Socket por el que queremos recibir UDP.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si todo ha ido correcto.
 *
 * ERRORES:
 *   La función devuelve -1 si ha ocurrido algún error.
 */
int udp_open( char* filename_conf, char* filename_rutas, uint16_t socket_recv);

/* void udp_close();
 *
 * DESCRIPCIÓN: 
 *   Libera toda la memoria abierta por la función udp_open().
 *
 */
void udp_close();

/* int udp_send 
 *	(ipv4_addr_t ipv4_dest, uint16_t socket_dest, char* payload, int payload_len );
 *
 * DESCRIPCIÓN:
 *   Esta función envia un datagrama UDP a la dirección IPv4 indicada por parametro.
 *	 Realiza el checksum de UDP.
 *
 * PARÁMETROS:
 *	'ipv4_dest': Dirección IPv4 destino del datagrama UDP.
 *
 *  'socket_dest': Puerto destino del datagrama UDP.
 *
 *	'payload': Carga que deseamos encapsular en el datagrama UDP.
 *
 *	'payload_len': Tamaño de la carga.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si se ha transmitido 2 veces la petición de REQUEST
 *   y no se ha obtenido respuesta o si se ha producido algún error durante el proceso.
 *
 */
int udp_send 
(ipv4_addr_t ipv4_dest, uint16_t socket_dest, char* payload, int payload_len );

/* int udp_recv 
 *	( uint16_t* socket_send, ipv4_addr_t ipv4_addr_src, unsigned char buff_out[], long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función .
 *
 * PARÁMETROS:
 *	'socket_send': Es el puerto origen del datagrama recibido.
 *
 *  'ipv4_addr_src': Dirección IPv4 del paquete recibido.
 *
 *	'buff_out[]': Aqui guardamos el contenido del datagrama UDP.
 *
 *	'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *             antes de retornar. Un número negativo indicará que debe
 *             esperarse indefinidamente, mientras que con un '0' la función
 *             retornará inmediatamente, se haya recibido o no una trama.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si se ha producido algún error durante el proceso.
 *
 */
int udp_recv 
( uint16_t* socket_send, ipv4_addr_t ipv4_addr_src, unsigned char buff_out[], long int timeout );

#endif
