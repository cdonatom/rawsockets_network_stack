#include "eth.h"
#include <stdio.h>
#include <stdlib.h>

#define TIMEOUT -1

int main ( int argc, char * argv[] ){
	/* Procesar parámetros */
	if (argc != 3) { printf("Uso: %s <iface> <tipo>\n", argv[0]); exit(-1); }

	/* Variables */
	mac_addr_t src_addr;
	unsigned char buffer[ETH_FRAME_MAX_SIZE];

	/* Recuperamos los parámetros */
	char* iface_name = argv[1];
	uint16_t eth_type = atoi(argv[2]);

	/* Abrir la interfaz Ethernet */
	eth_iface_t * eth_iface = eth_open(iface_name);
	if (eth_iface == NULL) {//Comprobamos que se haya abierto correctamente el ifaz.
		printf ("Ha habido un error al abrir la interfaz %s\n",argv[1]); 
		exit(-1); 
	}

	while(1) {
		/* Recibir trama Ethernet del Cliente */
		printf("Escuchando tramas Ethernet (tipo=0x%04x) ...\n", eth_type);
		int payload_len = eth_recv(eth_iface, src_addr, eth_type, buffer, TIMEOUT);
		if (payload_len == -1) { printf("Ha habido un error al recibir.\n"); break; }
		
		/* Enviar la misma trama Ethernet de vuelta al Cliente */
		int len = eth_send(eth_iface, src_addr, eth_type, buffer, payload_len);
		if (len == -1) { break; }
		printf("Enviado %d bytes al Cliente Ethernet:\n", payload_len);
	}
	/* Cerrar interfaz Ethernet */
	eth_close(eth_iface);
	return 0;
}
