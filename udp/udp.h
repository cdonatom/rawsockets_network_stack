#ifndef _UDP_H
#define _UDP_H 

#include "../ipv4/ipv4.h"
#include <stdint.h>

#define UDP_HEADER_SIZE 8

/* int udp_open( char* filename_conf, char* filename_rutas, uint16_t socket_recv);
 *
 * DESCRIPCI�N: 
 *   Esta funci�n asigna el socket indicado por par�metros e incializa todo lo necesario
 *	de IP para que �ste funcione.
 *
 * PAR�METROS:
 *   'filename_conf': Nombre del fichero donde se encuentra nuestra configuraci�n,
 *					es decir, nuestra interfaz, direcci�n IP y m�scara de subred.
 *
 *  'filename_routas': Nombre del fichero donde se encuentran las rutas IP que se van a a�adir
 *					a la tabla de rutas IP.
 *
 *	'socket_recv': Socket por el que queremos recibir UDP.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si todo ha ido correcto.
 *
 * ERRORES:
 *   La funci�n devuelve -1 si ha ocurrido alg�n error.
 */
int udp_open( char* filename_conf, char* filename_rutas, uint16_t socket_recv);

/* void udp_close();
 *
 * DESCRIPCI�N: 
 *   Libera toda la memoria abierta por la funci�n udp_open().
 *
 */
void udp_close();

/* int udp_send 
 *	(ipv4_addr_t ipv4_dest, uint16_t socket_dest, char* payload, int payload_len );
 *
 * DESCRIPCI�N:
 *   Esta funci�n envia un datagrama UDP a la direcci�n IPv4 indicada por parametro.
 *	 Realiza el checksum de UDP.
 *
 * PAR�METROS:
 *	'ipv4_dest': Direcci�n IPv4 destino del datagrama UDP.
 *
 *  'socket_dest': Puerto destino del datagrama UDP.
 *
 *	'payload': Carga que deseamos encapsular en el datagrama UDP.
 *
 *	'payload_len': Tama�o de la carga.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La funci�n devuelve -1 si se ha transmitido 2 veces la petici�n de REQUEST
 *   y no se ha obtenido respuesta o si se ha producido alg�n error durante el proceso.
 *
 */
int udp_send 
(ipv4_addr_t ipv4_dest, uint16_t socket_dest, char* payload, int payload_len );

/* int udp_recv 
 *	( uint16_t* socket_send, ipv4_addr_t ipv4_addr_src, unsigned char buff_out[], long int timeout );
 *
 * DESCRIPCI�N:
 *   Esta funci�n .
 *
 * PAR�METROS:
 *	'socket_send': Es el puerto origen del datagrama recibido.
 *
 *  'ipv4_addr_src': Direcci�n IPv4 del paquete recibido.
 *
 *	'buff_out[]': Aqui guardamos el contenido del datagrama UDP.
 *
 *	'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *             antes de retornar. Un n�mero negativo indicar� que debe
 *             esperarse indefinidamente, mientras que con un '0' la funci�n
 *             retornar� inmediatamente, se haya recibido o no una trama.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La funci�n devuelve -1 si se ha producido alg�n error durante el proceso.
 *
 */
int udp_recv 
( uint16_t* socket_send, ipv4_addr_t ipv4_addr_src, unsigned char buff_out[], long int timeout );

#endif
