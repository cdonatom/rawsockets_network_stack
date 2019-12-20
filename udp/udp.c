#include "udp.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <timerms.h>

#define UDP_LAYER_PROT 0x11
#define UDP_PAYLOAD_LEN (IPv4_PAYLOAD_LEN - UDP_HEADER_SIZE)
#define UDP_PSEUDOHEADER_SIZE (UDP_HEADER_SIZE + 12)

uint16_t my_socket;

struct udp_header {
	uint16_t socket_src;//2 bytes
	uint16_t socket_dest;//2 bytes
	uint16_t datagram_len;//2 bytes
	uint16_t checksum;//2 bytes
	char payload[UDP_PAYLOAD_LEN];
};

typedef struct udp_header udp_header_t;

//Pseudocabecera UDP-IPv4 para el cálculo del cheksum
struct udp_pseudoheader_ipv4{
	ipv4_addr_t src_addr; // 4bytes
	ipv4_addr_t dest_addr;// 4bytes
	uint8_t zeros; //1 byte
	uint8_t protocol; //1 byte
	uint16_t udp_len; // 2 bytes
	udp_header_t udp_datagram;
};

/* void print_datagram_udp( udp_header_t * udp_datagram);
 *
 * DESCRIPCIÓN: 
 *   Función que se utiliza para mostrar por pantalla la estructura de cabecera de un 
 *	segmento UDP dado.
 *
 * PARÁMETROS:
 *   'udp_datagram': Datagrama UDP que queremos mostrar.
 *
 */
#ifdef DEBUG
void print_datagram_udp( udp_header_t * udp_datagram){
	printf("\nUDP: socket source = %i\n", ntohs(udp_datagram-> socket_src));
	printf("UDP: socket dest = %i\n", ntohs(udp_datagram-> socket_dest));
	printf("UDP: datagram length = %i\n", ntohs(udp_datagram-> datagram_len));
	printf("UDP: checksum datagram = 0x%04x \n\n", ntohs(udp_datagram-> checksum));
}
#endif

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
int udp_open( char* filename_conf, char* filename_rutas, uint16_t socket_recv){
	int err;
	my_socket = socket_recv;	
	err = ipv4_open(filename_conf, filename_rutas);
	return err;
}

/* void udp_close();
 *
 * DESCRIPCIÓN: 
 *   Libera toda la memoria abierta por la función udp_open().
 *
 */
void udp_close(){
	ipv4_close();
}

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
(ipv4_addr_t ipv4_dest, uint16_t socket_dest, char* payload, int payload_len ){
	if ( payload_len > UDP_PAYLOAD_LEN ){
		printf("Payload demasiado grande.\n");
		printf("Esta versión de UDP e IPv4 no permite fragmentacion... Por ahora.\n");
		return -1;
	}
	int err = 0;
	udp_header_t udp_datagram;
	ipv4_addr_t my_ip;
	get_my_ip(my_ip);
	struct udp_pseudoheader_ipv4 udp_ipv4;
	uint16_t checksum;
	
	memset(&udp_datagram, 0, sizeof(udp_header_t));
	memset(&udp_ipv4, 0, sizeof(struct udp_pseudoheader_ipv4));
		
	udp_datagram.socket_src = htons(my_socket);
	udp_datagram.socket_dest = htons(socket_dest);
	udp_datagram.datagram_len = htons(payload_len + UDP_HEADER_SIZE);
	udp_datagram.checksum = 0;
	memcpy( udp_datagram.payload, payload, payload_len);
	memcpy( udp_datagram.payload, payload, payload_len);
	//Rellenamos la pseudocabecera.
	memcpy( udp_ipv4.src_addr, my_ip, IPv4_ADDR_SIZE);
	memcpy( udp_ipv4.dest_addr, ipv4_dest, IPv4_ADDR_SIZE);
	udp_ipv4.zeros=0;
	udp_ipv4.protocol = UDP_LAYER_PROT;
	udp_ipv4.udp_len = htons(payload_len + UDP_HEADER_SIZE);
	memcpy( (void*)&(udp_ipv4.udp_datagram), (void*) &udp_datagram, sizeof(udp_datagram));
		
	checksum = ipv4_checksum((void *) &udp_ipv4, UDP_PSEUDOHEADER_SIZE + payload_len);

	udp_datagram.checksum = htons(checksum);
	
	err = ipv4_send
	(ipv4_dest, UDP_LAYER_PROT, (void*)&udp_datagram, UDP_HEADER_SIZE + payload_len);
	if (err == -1){
		printf("Ha habido un error al enviar el datagrama UDP.\n");
		return -1;
	}
#ifdef DEBUG
	printf(" Datagrama UDP ENVIADO:\n");
	print_datagram_udp(&udp_datagram);
#endif
	return err;
}

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
( uint16_t* socket_send, ipv4_addr_t ipv4_addr_src, unsigned char buff_out[], long int timeout ){
	
	int err;
	udp_header_t udp_datagram;
	struct udp_pseudoheader_ipv4 udp_ipv4;
	memset(&udp_datagram, 0, sizeof(udp_header_t));
	memset(&udp_ipv4, 0, sizeof(struct udp_pseudoheader_ipv4));
	timerms_t timer;
	timerms_reset(&timer, timeout);
	
	uint16_t checksum, checksum_recv;
	ipv4_addr_t my_ip;
	get_my_ip(my_ip);
	
	long int time_left = 0;
	
	do{
		time_left = timerms_left(&timer);
		err = ipv4_recv( UDP_LAYER_PROT, ipv4_addr_src, (void*)&(udp_datagram), time_left);

		if ( err == -1 ){
			printf("Ha habido un problema al recibir el datagrama UDP.\n");
			return -1;
		}
		if( err >= UDP_HEADER_SIZE){
			if (ntohs (udp_datagram.checksum) == 0){ //Checksum es opcional
				if( ntohs(udp_datagram.socket_dest) == my_socket){
					uint16_t socket = ntohs(udp_datagram.socket_src);
#ifdef DEBUG
					printf(" Datagrama UDP RECIBIDO:\n");
					print_datagram_udp(&udp_datagram);
#endif		
					memcpy(buff_out, udp_datagram.payload, (ntohs(udp_datagram.datagram_len) - UDP_HEADER_SIZE));
					memcpy(socket_send, (void*) &socket, sizeof(uint16_t));
					return (ntohs(udp_datagram.datagram_len) - UDP_HEADER_SIZE);			
				}
			}
			else{ //Hacemos el checksum para comprobar que el datagrama está bien.
				memset((void*) &(udp_ipv4), 0, ntohs(udp_datagram.datagram_len)+UDP_PSEUDOHEADER_SIZE);
				memcpy((void*)&(udp_ipv4.udp_datagram), (void*)&(udp_datagram), ntohs(udp_datagram.datagram_len));
				checksum_recv = ntohs(udp_datagram.checksum);
#ifdef DEBUG
				printf("Checksum_recv = 0x%04x \n", checksum_recv);
#endif			
				udp_ipv4.udp_datagram.checksum = 0;
				memcpy(udp_ipv4.src_addr, ipv4_addr_src, IPv4_ADDR_SIZE);
				memcpy(udp_ipv4.dest_addr, my_ip, IPv4_ADDR_SIZE);
				udp_ipv4.protocol = UDP_LAYER_PROT;
				udp_ipv4.zeros = 0x00;
				udp_ipv4.udp_len = udp_datagram.datagram_len;
				checksum = ipv4_checksum((void *) &(udp_ipv4), UDP_PSEUDOHEADER_SIZE + ntohs(udp_ipv4.udp_len));
#ifdef DEBUG
				printf("Checksum = 0x%04x \n", checksum);
#endif
				if (checksum == checksum_recv){
					if( ntohs(udp_datagram.socket_dest) == my_socket){
						uint16_t socket = ntohs(udp_datagram.socket_src);
#ifdef DEBUG
						printf(" Datagrama UDP RECIBIDO:\n");
						print_datagram_udp(&udp_datagram);
#endif		
						memcpy(buff_out, udp_datagram.payload, (ntohs(udp_datagram.datagram_len) - UDP_HEADER_SIZE));
						memcpy(socket_send, (void*) &socket, sizeof(uint16_t));
						return (ntohs(udp_datagram.datagram_len) - UDP_HEADER_SIZE);			
					}
				}
				else{
					printf("Datagrama corrupto.\n");
					return -1;
				}
			}
		}
		
/*		if( err >= UDP_HEADER_SIZE){
			if( ntohs(udp_datagram.socket_dest) == my_socket){
				uint16_t socket = ntohs(udp_datagram.socket_src);
#ifdef DEBUG
				printf(" Datagrama UDP RECIBIDO:\n");
				print_datagram_udp(&udp_datagram);
#endif		
				memcpy(buff_out, udp_datagram.payload, (ntohs(udp_datagram.datagram_len) - UDP_HEADER_SIZE));
				memcpy(socket_send, (void*) &socket, sizeof(uint16_t));
				return (ntohs(udp_datagram.datagram_len) - UDP_HEADER_SIZE);			
			}
		}*/
	}
	while(time_left > 0 || time_left < -1);
	
	printf("No se ha recibido ningún datagrama UDP correcto.\n");
	return -1;
}
