#include "arp.h"
#include "../ipv4/ipv4.h"

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#define TIMEOUT_SERVER_ARP -1
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


int main ( int argc, char * argv[] ){
	/* Procesar parámetros */
	if (argc != 2) { printf("Uso: %s <iface> \n", argv[0]); exit(-1); }

	/* Definición de las variables */
	mac_addr_t mac_addr;
	mac_addr_t my_mac;
	ipv4_addr_t my_ip;
	char* iface_name = argv[1];
	eth_iface_t * my_iface = eth_open ( iface_name );
	unsigned char buffer [ETH_MTU];
	int payload_len;
	arp_header_t arp_frame;
	arp_header_t* arp_frame_ptr=(void*) buffer;
	char* filename_conf="ipv4/config/ipv4_config_server.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_server.txt";
	
	payload_len=ipv4_open(filename_conf, filename_rutas);
	get_my_ip(my_ip);
	if(payload_len == -1){
		printf("Ha habido un error al abrir IPv4. \n");
		exit(-1);
	}
	eth_getaddr( my_iface, my_mac );//Obtenemos nuestra dirección Mac.
	
	while(1){
		
		payload_len=eth_recv
		(my_iface, mac_addr, ARP_TYPE, buffer, TIMEOUT_SERVER_ARP);
		
		if( payload_len == -1 ){ 
			printf("Ha habido un error al recibir la trama ARP.\n");
		}		

#ifdef DEBUG
		printf("\n Log de la trama ARP RECIBIDA\n");
		print_pkt_arp(arp_frame_ptr);
#endif		
		//Comprobamos que el mensaje recibido es para nosotros:
		if(ntohs(arp_frame_ptr->opcode) == REQUEST 
			&& (memcmp(arp_frame_ptr->dest_hard_prot, my_ip, IPv4_ADDR_SIZE)==0)){
			
			arp_frame.hardware_type=htons(ETH_HARD_TYPE);
			arp_frame.protocol_type=htons(IPv4_PROT_TYPE);
			arp_frame.hard_addr_len=MAC_ADDR_SIZE;
			arp_frame.prot_addr_len=IPv4_ADDR_SIZE;
			arp_frame.opcode=htons(REPLY);
			memcpy(arp_frame.src_hard_addr, my_mac, sizeof(mac_addr_t));	
			memcpy(arp_frame.src_hard_prot, my_ip, sizeof(ipv4_addr_t));
			memcpy(arp_frame.dest_hard_addr, arp_frame_ptr->src_hard_addr, sizeof(mac_addr_t));
			memcpy(arp_frame.dest_hard_prot, arp_frame_ptr->src_hard_prot, sizeof(ipv4_addr_t));
			
			payload_len=eth_send( my_iface, arp_frame_ptr->src_hard_addr, ARP_TYPE, (void*)&arp_frame, ARP_HEADER_SIZE);

#ifdef DEBUG
			printf("\n Log de la trama ARP a ENVIAR\n");
			print_pkt_arp(&arp_frame);
#endif			
		}		
	}
}
