#include "arp.h"
#include <stdio.h>
#include <stdlib.h>


int main ( int argc, char * argv[] ){
	
	/* Procesar argumentos */
	if (argc != 3) {
		printf("Uso: %s <iface> <ipv4_destino>\n", argv[0]); 
		exit(-1);
	}

	/* Variables */
	char* iface_name = argv[1];
	eth_iface_t * iface = eth_open ( iface_name );//Inicializa la interfaz.
	char* ip_addr_str= argv[2];
	char* filename_conf="ipv4/config/ipv4_config_client.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_client.txt";
	mac_addr_t mac_addr;
	char mac_addr_str[MAC_ADDR_STR_LENGTH];
	ipv4_addr_t ip_addr;
	int err;

	err = ipv4_str_addr( ip_addr_str, ip_addr);//Convertirmos el String a estructura
	if(err == -1){
		printf("Direccion IP invalida. \n");
		exit(-1);
	}
	err= ipv4_open(filename_conf, filename_rutas);
	
	if(err == -1){
		printf("Ha habido un error al abrir IPv4. \n");
		exit(-1);
	}
	
	err = arp_resolve(iface, ip_addr, mac_addr);	
	if (err == -1) {  //Comprobamos que se haya resuelto correctamente.
		printf ("Ha habido un error al resolver la dirección IP\n"); 
		ipv4_close();
		exit(-1); 
	}

	eth_mac_str(mac_addr, mac_addr_str);
	printf("%s -> %s\n", ip_addr_str, mac_addr_str);//Sacamos la Mac obtenida.
	//Volvemos a hacer una petición para comprobar que se guardó la Mac en la cache.
	err = arp_resolve(iface, ip_addr, mac_addr);
	printf("%s -> %s\n", ip_addr_str, mac_addr_str);//Sacamos la Mac obtenida.
	ipv4_close();
	return 0;
}
