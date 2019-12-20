#ifndef _IPv4_H
#define _IPv4_H

#include "../eth/eth.h"
#include <stdint.h>

#define IPv4_ADDR_SIZE 4
#define IPv4_ADDR_STR_LENGTH 16
#define IPv4_HEADER_SIZE 20

#define IPv4_PAYLOAD_LEN (ETH_MTU - IPv4_HEADER_SIZE)

typedef unsigned char ipv4_addr_t [IPv4_ADDR_SIZE];

/* Dirección IPv4 vacía */
extern ipv4_addr_t EMPTY_IPv4;

/* void get_my_ip(ipv4_addr_t ipv4_addr);
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve nuestra dirección IPv4.
 *
 * PARÁMETROS:
 *	'ipv4_addr': Dirección donde queremos que se copie nuestra IP.
 *
 */
void get_my_ip(ipv4_addr_t ipv4_addr);

/* void get_my_ip_bcast(ipv4_addr_t ipv4_addr);
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve nuestra dirección de broadcast.
 *
 * PARÁMETROS:
 *	'ipv4_addr': Dirección donde queremos que se copie nuestra dirección broadcast.
 *
 */
void get_my_ip_bcast(ipv4_addr_t ipv4_addr);

/* void * get_my_table();
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve nuestra tabla de direcciones IPv4.
 *
 */
void* get_my_table();

/* char* get_my_iface();
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve el interfaz a la que pertenece esta IPv4.
 *
 */
char* get_my_iface();

/* int ipv4_open(char* filename_config, char* filename_routes);
 *
 * DESCRIPCIÓN:
 *   Con esta funcion obtenemos nuestra interfaz, direccion IP y máscara de subred. 
 *	Tambien obtenemos nuestra dirección de bradcast e inicializamos la interfaz y la tabla de rutas.
 *
 * PARÁMETROS:
 *	'filename_config': Nombre del fichero donde se encuentra nuestra configuración,
 *					es decir, nuestra interfaz, dirección IP y máscara de subred.
 *
 *  'filename_routes': Nombre del fichero donde se encuentran las rutas IP que se van a añadir
 *					a la tabla de rutas IP.
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si ha habido algun error durante el proceso.
 *
 */
int ipv4_open(char* filename_config, char* filename_routes);

/* int ipv4_close();
 *
 * DESCRIPCIÓN:
 *   Con esta funcion liberamos toda la memoria reservada en la función epv4_open().
 *
 * VALOR DEVUELTO:
 *  Devuelve un 0 si todo ha ido correctamente.
 * 
 * ERRORES:
 *   La función devuelve -1 si ha habido algun error durante el proceso.
 *
 */
int ipv4_close();

/* void calculate_ipv4_broadcast 
 *	(ipv4_addr_t addr, ipv4_addr_t subnet, ipv4_addr_t* res_bcast);
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener la dirección IPv4 de broadcast dando una
 *	subred y una dirección IPv4 de esa subred.
 *
 * PARÁMETROS:
 *	'addr': Dirección IPv4.
 *
 *  'subnet': subred de la que deseamos obtener la dirección broadcast.
 *
 *	'res_bcast': direccion IPv4 donde guardaremos la dirección broadcast obtenida.
 *
 *
 * VALOR DEVUELTO:
 *  Devuelve la dirección de broadcast. Si la subred o la dirección IPv4 indicadas son
 *	NULL, entonces devolverá una dirección IPv4 NULL.
 * 
 */
void calculate_ipv4_broadcast (ipv4_addr_t addr, ipv4_addr_t subnet, ipv4_addr_t* res_bcast);

/*
 * uint16_t ipv4_checksum ( unsigned char * data, int len )
 *
 * DESCRIPCIÓN:
 *   Esta función calcula el checksum IP de los datos especificados.
 *
 * PARÁMETROS:
 *   'data': Puntero a los datos sobre los que se calcula el checksum.
 *    'len': Longitud en bytes de los datos.
 *
 * VALOR DEVUELTO:
 *   El valor del checksum calculado.
 */
uint16_t ipv4_checksum ( unsigned char * data, int len );

/* void ipv4_addr_str ( ipv4_addr_t addr, char* str );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección IPv4
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección IP que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'IP_ADDR_STR_LENGTH' bytes.
 */
void ipv4_addr_str ( ipv4_addr_t addr, char* str );

/* int ipv4_str_addr ( char* str, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección IPv4.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección IPv4 encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección IPv4.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección IPv4.
 */
int ipv4_str_addr ( char* str, ipv4_addr_t addr );

/* int ipv4_recv
 *	(uint8_t layer_prot, ipv4_addr_t ipv4_addr_src, unsigned char * buff_out , long int timeout);
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener el siguiente paquete recibido por la
 *   interfaz Ethernet indicada. La operación puede esperar indefinidamente o
 *   un tiempo limitando dependiento del parámetro 'timeout'. Recibe tambien los
 *	 paquetes enviados a su dirección de broadcast. Realiza comprobación de checksum.
 *
 *   Esta función sólo permite recibir paquetes de una única interfaz. Si desea
 *   escuchar varias interfaces Ethernet simultaneamente, utilice la función
 *   'eth_poll()'.
 *
 * PARÁMETROS:
 *    'layer_prot': Indica el tipo de protocolo que queremos que tenga el paquete a recibir.
 * 		
 *	  'ipv4_addr_src': Dirección IPv4 donde se guarda la dirección de origen del paquete recibido.
 *   
 *    'buff_out': Buffer donde se guarda el paquete IP recibido.
 *
 *    'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *             antes de retornar. Un número negativo indicará que debe
 *             esperarse indefinidamente, mientras que con un '0' la función
 *             retornará inmediatamente, se haya recibido o no una trama.
 *
 * VALOR DEVUELTO:
 *   La longitud en bytes de los datos de la trama recibida, o '0' si no se ha
 *   recibido ninguna trama porque ha expirado el temporizador.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int ipv4_recv
(uint8_t layer_prot, ipv4_addr_t ipv4_addr_src, unsigned char * buff_out , long int timeout);

/* int ipv4_send
 *	(ipv4_addr_t ip_addr_dest, uint8_t layer_prot, char* payload, int payload_len);
 *
 * DESCRIPCIÓN:
 *   Esta función encapsula un payload dado en un paquete IP y lo envía al destino indicado.
 *	Lo primero que hace es buscar la ruta óptima por donde enviar el paquete. Para ello mira
 *	en la tabla de rutas y compara con cual obtiene el prefijo más largo. Una vez obtenida la
 *	ruta óptima, se resuelve la dirección por ARP y se envía a través de ethernet.
 *
 * PARÁMETROS:
 *    'ip_addr_dest': Dirección IPv4 de destino del paquete.
 * 		
 *	  'layer_prot': Indicador del protocolo del paquete a enviar.
 *   
 *    'payload': Contenido del paquete IP que queremos enviar.
 *
 *    'payload_len': Longitud del contenido del paquete.
 *
 * VALOR DEVUELTO:
 *   La longitud en bytes de los datos del paquete enviado.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int ipv4_send
(ipv4_addr_t ip_addr_dest, uint8_t layer_prot, char* payload, int payload_len);

#endif /* _IPv4_H */
