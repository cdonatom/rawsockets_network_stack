#include "arp.h"
#include "arp_cache.h"

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <arpa/inet.h>
#include <timerms.h>

#define ARP_TYPE 0x0806
#define TIMEOUT_ARP_1 2000
#define TIMEOUT_ARP_2 3000
#define IPv4_PROT_TYPE 0x0800
#define ARP_HEADER_SIZE 28
#define ARP_PROT_SIZE 1
#define REQUEST 1
#define REPLY 2


/* Cabecera de una trama ARP */
struct arp_header {
	uint16_t hardware_type; //4 bytes
	uint16_t protocol_type; //4 bytes
	uint8_t hard_addr_len;//2 bytes
	uint8_t prot_addr_len;//2 bytes
	uint16_t opcode;//4 bytes
	mac_addr_t src_hard_addr;//6 bytes
	ipv4_addr_t src_hard_prot;//4 bytes
	mac_addr_t dest_hard_addr;//6 bytes
	ipv4_addr_t dest_hard_prot;//4 bytes
};

typedef struct arp_header arp_header_t;

arp_cache_table_t* arp_cache;
int last_index = 0;


/* void print_pkt_arp(arp_header_t * arp_frame) 
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por pantalla toda la estructura de una trama arp indicada.
 *
 * PARÁMETROS:
 *	'arp_frame': trama arp indicada mediante puntero de la que se quiere presentar 
 *				su estructura.
 *
 */
#ifdef DEBUG
void print_pkt_arp(arp_header_t * arp_frame){			
	char my_mac_str[MAC_ADDR_STR_LENGTH];
	char dest_mac_str[MAC_ADDR_STR_LENGTH];
	char ip_dest_str[IPv4_ADDR_STR_LENGTH];
	char ip_src_str[IPv4_ADDR_STR_LENGTH];
		
	eth_mac_str(arp_frame->src_hard_addr, my_mac_str);
	eth_mac_str(arp_frame->dest_hard_addr, dest_mac_str);
	ipv4_addr_str (arp_frame->dest_hard_prot, ip_dest_str);
	ipv4_addr_str (arp_frame->src_hard_prot, ip_src_str);

	//Imprimimos toda la estructura:
	printf("ARP: Hardware type = %i\n", ntohs(arp_frame->hardware_type));
	printf("ARP: Protocol type= 0x%04x\n", ntohs(arp_frame->protocol_type) );
	printf("ARP: Hardware Address Length = %i\n", arp_frame->hard_addr_len);
	printf("ARP: Protocol Address Length = %i\n", arp_frame->prot_addr_len);
	printf("ARP: Opcode = %i\n", ntohs(arp_frame->opcode));
	printf("ARP: Source Hardware Address = %s\n", my_mac_str);
	printf("ARP: Source Protocol Address = %s\n", ip_src_str);
	printf("ARP: Dest Hardware Address = %s\n",dest_mac_str);
	printf("ARP: Dest Protocol Address = %s\n\n",ip_dest_str);
}
#endif

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
int arp_open(){
	arp_cache = create_cache_table();
	if (arp_cache != NULL )
		return 0;
	else
		return -1;
}

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
int arp_close (){
	return free_table (arp_cache);
}

/* arp_cache_table_t* get_my_cache();
 *
 * DESCRIPCIÓN:
 *   Esta función devuelve la tabla cache de arp.
 *
 * VALOR DEVUELTO:
 *  Devuelve la tabla cache.
 * 
 */
arp_cache_table_t* get_my_cache(){
	return arp_cache;
}

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
(eth_iface_t * iface, ipv4_addr_t ip_addr, mac_addr_t mac_addr){

	int i;
	for (i=0; i<ARP_CACHE_NUM_ENTRYS; i++){
		if( memcmp(ip_addr, (void*)&(arp_cache->arp_entrys[i].ipv4_addr), IPv4_ADDR_SIZE) == 0){
#ifdef DEBUG
	printf("Tengo una entrada para esta IPv4 en mi tabla cache de ARP!.\n");
#endif 
			memcpy(mac_addr, (void*)&(arp_cache->arp_entrys[i].mac_addr), MAC_ADDR_SIZE);
			return 0;
		}

	}
#ifdef DEBUG
	printf("La tabla cache está vacia o no tiene esta direccion IPv4.\n");
#endif 
	if(last_index == ARP_CACHE_NUM_ENTRYS)
		last_index = 0;
	/* Variables */	
	int err=0;
	unsigned char buffer[ETH_MTU];

	/* Creamos una estructura para el datagrama */
	arp_header_t arp_frame;
	arp_header_t* arp_frame_ptr;
	
	/* Inicializamos nuestra estructura */
	ipv4_addr_t my_ip;
	get_my_ip(my_ip);//Obtenemos nuestra ip.
	mac_addr_t my_mac;
	eth_getaddr(iface, my_mac);//Obtenemos nuestra mac.
	
	/* Rellenamos la estructura con los datos del protocolo. */
	arp_frame.hardware_type=htons(ETH_HARD_TYPE);
	arp_frame.protocol_type=htons(IPv4_PROT_TYPE);
	arp_frame.hard_addr_len=MAC_ADDR_SIZE;
	arp_frame.prot_addr_len=IPv4_ADDR_SIZE;
	arp_frame.opcode=htons(REQUEST);
	memcpy(arp_frame.src_hard_addr, my_mac, MAC_ADDR_SIZE);	
	memcpy(arp_frame.src_hard_prot, my_ip, IPv4_ADDR_SIZE);
	memcpy(arp_frame.dest_hard_addr, EMPTY_MAC_ADDR, MAC_ADDR_SIZE);
	memcpy(arp_frame.dest_hard_prot, ip_addr, IPv4_ADDR_SIZE);
	
	/* Inicializamos nuestro timer. */
	timerms_t timer;
	timerms_reset(&timer, TIMEOUT_ARP_1);
	long int time_left;
		
	/* Enviamos un Broadcast con la direccion IP encapsulada */
	err=eth_send( iface, BROADCAST_MAC_ADDR, ARP_TYPE, (void*)&arp_frame, ARP_HEADER_SIZE);

#ifdef DEBUG
	printf("\n Log de la trama ARP a ENVIAR\n");
	print_pkt_arp(&arp_frame); // imprimimos el datagrama que vamos a enviar
#endif

	if (err == -1){
		printf("Ha habido un error al enviar el paquete ARP.\n");	
		return -1;
	}
	//Esperamos 2 segundos a recibir respuesta a nuestra petición.
	do{
		time_left=timerms_left(&timer);

		if(time_left == -1){
			printf("No se ha inicializado correctamente el timer. \n");
			return -1;
		}
		/* Esperamos la respuesta a nuestra petición. */
		err=eth_recv
		(iface, mac_addr, ARP_TYPE, buffer, time_left); //Esperamos 2 segundos.	
#ifdef DEBUG
		printf("Retorno eth_recv: %i\n", err);
#endif	
		if (err == -1){
			printf("Ha habido un error al recibir el paquete ARP.\n");	
			return -1;
		}
		if( err >= ARP_HEADER_SIZE){
			arp_frame_ptr=(arp_header_t *) buffer;
#ifdef DEBUG
			printf("\n Log de la trama ARP RECIBIDA\n");
			print_pkt_arp(arp_frame_ptr);
#endif	
			if 
			(ntohs(arp_frame_ptr->opcode) == REPLY
			&& (memcmp(arp_frame_ptr->dest_hard_prot, arp_frame.src_hard_prot, IPv4_ADDR_SIZE) == 0)
			&& (memcmp(arp_frame_ptr->src_hard_prot, arp_frame.dest_hard_prot,IPv4_ADDR_SIZE)==0)){
				memcpy(mac_addr,arp_frame_ptr->src_hard_addr, MAC_ADDR_SIZE);
				arp_cache_table_add 
				(arp_cache, ip_addr, arp_frame_ptr->src_hard_addr, last_index);
				last_index++;
#ifdef DEBUG
				printf("Aniado esta IPv4 a la tabla cache de ARP.\n");
#endif 
				return 0;
			}
			else
				err=0;
		}
	}
	while(time_left > 0);

	if (err == 0){
		timerms_reset(&timer, TIMEOUT_ARP_2); //Reseteamos el timer para contar 3 seg.
		/* Retrasmitimos la petición. */
		err=eth_send(iface, BROADCAST_MAC_ADDR, ARP_TYPE, (void*)&arp_frame, ARP_HEADER_SIZE);

#ifdef DEBUG
		printf("\n Log de la trama ARP a ENVIAR\n");
		print_pkt_arp(&arp_frame);
#endif
		if (err == -1){
			printf("Ha habido un error al enviar el paquete ARP.\n");	
			return -1;
		}
#ifdef DEBUG
		printf("Ha fallado el primer intento de resolver la dirección MAC.\n");
#endif
		//Esperamos 3 segundos a recibir respuesta a nuestra petición.
		do{	
			time_left=timerms_left(&timer);
			if(time_left == -1){
				printf("No se ha inicializado correctamente el timer. \n");
				return -1;
			}
			/* Esperamos la respuesta a nuestra petición. */
			err=eth_recv
			(iface, mac_addr, ARP_TYPE, buffer, time_left); //Esperamos 2 segundos.	
#ifdef DEBUG
			printf("Retorno eth_recv: %i\n", err);
#endif	
			if (err == -1){
				printf("Ha habido un error al recibir el paquete ARP.\n");	
				return -1;
			}
			if( err <= ARP_HEADER_SIZE){
				arp_frame_ptr=(arp_header_t *) buffer;
#ifdef DEBUG
			printf("\n Log de la trama ARP RECIBIDA\n");
			print_pkt_arp (arp_frame_ptr );
#endif	
				if 
				(ntohs(arp_frame_ptr->opcode) == REPLY
				&& (memcmp(arp_frame_ptr->dest_hard_prot, arp_frame.src_hard_prot, IPv4_ADDR_SIZE) == 0)
				&& (memcmp(arp_frame_ptr->src_hard_prot, arp_frame.dest_hard_prot,IPv4_ADDR_SIZE)==0)){
					memcpy(mac_addr ,arp_frame_ptr->src_hard_addr, MAC_ADDR_SIZE);
					arp_cache_table_add 
					(arp_cache, ip_addr, arp_frame_ptr->src_hard_addr, last_index);
					last_index++;
#ifdef DEBUG
					printf("Aniado esta IPv4 a la tabla cache de ARP.\n");
#endif 
					return 0;
				}
				else
					err=0;
			}
		}
		while(time_left > 0);
	}
	
	printf("Se ha retransmitido 2 veces la peticion ARP\ny ha expirado el tiempo de respuesta.\n");
	return -1;
}
