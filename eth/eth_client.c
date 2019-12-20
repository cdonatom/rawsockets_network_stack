#include "eth.h"
#include <stdio.h>
#include <stdlib.h>

int main ( int argc, char * argv[] ){
	
	/* Procesar argumentos */
	if (argc != 5) {
		printf("Uso: %s <iface> <tipo> <mac> <long>\n", argv[0]); 
		exit(-1);
	}
	
	/* Variables */
	int err,len;
	mac_addr_t server_addr , src_addr;
	unsigned char buffer[ETH_FRAME_MAX_SIZE];
	char myself[MAC_ADDR_STR_LENGTH];

	/* Obtenemos los parámetros */
	char* iface_name=argv[1];
	uint16_t eth_type=atoi(argv[2]);
	err=eth_str_mac (argv[3], server_addr );
	int payload_len=atoi (argv[4]);
	unsigned char payload [payload_len];

	if( err== -1){ printf("MAC mal introducida\n"); exit(-1); } //Comprobamos que la MAC sea correcta.

	/* Abrir la interfaz Ethernet */
	eth_iface_t * eth_iface = eth_open(iface_name);

	if (eth_iface == NULL) {  //Comprobamos que se haya abierto correctamente el ifaz.
		printf ("Ha habido un error al abrir la interfaz %s\n",argv[1]); 
		exit(-1); 
	}
	eth_getaddr(eth_iface, src_addr); //Obtenemos nuestra MAC.
	eth_mac_str ( src_addr, myself ); //Convertimos la MAC a char* en myself

	int i;
	for (i=0; i < payload_len; i++)
		payload[i]=i; //Rellenamos el mensaje con 0.
	
	/* Enviar trama Ethernet al Servidor */
	err = eth_send(eth_iface, server_addr, eth_type, payload, payload_len);

	if (err == -1) { printf("Ha habido un error al enviar el paquete \n"); exit(-1); } //Comprobamos la transmisiópn del paquite
	
	/* Recibir trama Ethernet del Servidor y procesar errores */
	len = eth_recv(eth_iface, src_addr, eth_type, buffer, 2000);
	if (len <= 0) { fprintf(stderr, "%s: ERROR en eth_recv()\n", myself);} 
	else { printf("Recibidos %d bytes del Servidor Ethernet\n", len); }
	
	/* Cerrar interfaz Ethernet */
	eth_close(eth_iface);
	return 0;
}
