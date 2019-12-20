#include "ipv4.h"
#include "../arp/arp.h"
#include "ipv4_config.h"
#include "ipv4_route_table.h"

#include <arpa/inet.h>
#include <timerms.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define IPv4_VERSION 4
#define DEFAULT_TTL 64
#define IPv4_PROT_TYPE 0x0800


/* Dirección IPv4 vacía */
ipv4_addr_t EMPTY_IPv4 = { 0x00, 0x00, 0x00, 0x00 };

ipv4_addr_t my_ip;
ipv4_addr_t netmask_my_ip;
ipv4_addr_t ip_bcast;
eth_iface_t * my_iface;
ipv4_route_table_t * my_table;

typedef struct ipv4_header {	
	uint8_t info_packet;//1 byte
	uint8_t type_service;//1 byte
	uint16_t packet_len;//2 bytes
	uint16_t id; //2 bytes
	uint16_t flag_offset; //2 bytes, No Frag
	uint8_t ttl; //1 byte
	uint8_t layer_prot; //1 byte
	uint16_t checksum; //2 bytes
	ipv4_addr_t src_ip_addr;//4 bytes
	ipv4_addr_t dest_ip_addr;//4 bytes
	unsigned char data[IPv4_PAYLOAD_LEN];	
} ipv4_header_t;

/* void print_pkt_ipv4(ipv4_header_t* ipv4_datagram)
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por pantalla toda la estructura de un datagrama ip dado.
 *
 * PARÁMETROS:
 *	'ipv4_datagram': datagrama que deseamos imprimir.
 *
 */

 #ifdef DEBUG
void print_pkt_ipv4(ipv4_header_t* ipv4_datagram){
	uint8_t ipv4_version=((ipv4_datagram->info_packet) &0xF0) >> 4;
	uint8_t ipv4_header_len=(ipv4_datagram->info_packet) & 0x0F;
	char ip_dest_str[IPv4_ADDR_STR_LENGTH];
	char ip_src_str[IPv4_ADDR_STR_LENGTH];
	
	ipv4_addr_str (ipv4_datagram->dest_ip_addr, ip_dest_str);
	ipv4_addr_str (ipv4_datagram->src_ip_addr, ip_src_str);
	
	printf("\nIPv4: version = %d\n", ipv4_version);
	printf("IPv4: header length = %d\n", ipv4_header_len);
	printf("IPv4: type of service = %i\n", ipv4_datagram->type_service);
	printf("IPv4: payload length = %i\n", ntohs(ipv4_datagram->packet_len));
	printf("IPv4: id = %i\n", ntohs(ipv4_datagram->id));
	printf("IPv4: flags = %i\n", ipv4_datagram->flag_offset);
	printf("IPv4: TimeToLive = %i\n", ipv4_datagram->ttl);
	printf("IPv4: Protocol = %i\n",ipv4_datagram->layer_prot);
	printf("IPv4: checksum = 0x%04x\n", ntohs(ipv4_datagram->checksum));
	printf("IPv4: source IPv4 = %s\n", ip_src_str );
	printf("IPv4: dest IPv4 = %s\n\n", ip_dest_str );
} 
 #endif

  /* void get_my_ip(ipv4_addr_t ipv4_addr);
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve nuestra dirección IPv4.
 *
 * PARÁMETROS:
 *	'ipv4_addr': Dirección donde queremos que se copie nuestra IP.
 *
 */
 
void get_my_ip(ipv4_addr_t ipv4_addr)
{
	memcpy(ipv4_addr, my_ip, IPv4_ADDR_SIZE);
}

/* void get_my_ip_bcast(ipv4_addr_t ipv4_addr);
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve nuestra dirección de broadcast.
 *
 * PARÁMETROS:
 *	'ipv4_addr': Dirección donde queremos que se copie nuestra dirección broadcast.
 *
 */

void get_my_ip_bcast(ipv4_addr_t ipv4_addr)
{
	memcpy(ipv4_addr, ip_bcast, IPv4_ADDR_SIZE);
}

/* void * get_my_table();
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve nuestra tabla de direcciones IPv4.
 *
 */

void * get_my_table(){
	return  (void*) my_table;
}

/* char* get_my_iface();
 *
 * DESCRIPCIÓN:
 *   Esta función nos devuelve el interfaz a la que pertenece esta IPv4.
 *
 */

char* get_my_iface(){
	return eth_getname ( my_iface );
}

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

int ipv4_open(char* filename_config, char* filename_routes)
{

	int err;
	char ifname[4];

//Obtenemos nuestra interfaz, nuestra IP y nuestra máscara de subred:
	err= ipv4_config_read (filename_config, ifname, my_ip, netmask_my_ip);
	if( err == -1){
		printf("Ha habido un error al abrir el fichero.\n");
		return -1;
	}
//Obtenemos nuestra dirección de broadcast:
	calculate_ipv4_broadcast (my_ip, netmask_my_ip, &ip_bcast);
	if( ip_bcast == NULL){
		printf("No se ha podido calcular la dirección de Broadcast para esta IPv4 y máscara.\n");
		return -1;
	}
	
//Abrimos la interfaz:
	my_iface=eth_open(ifname);
	if(my_iface == NULL){
		printf("Ha habido un error al abrir el interfaz.\n");
		return -1;
	}
//Creamos la tabla de rutas vacía y la rellenamos:
	my_table=ipv4_route_table_create();
	err=ipv4_route_table_read (filename_routes, my_table);
#ifdef DEBUG
	ipv4_route_table_print(my_table);
#endif
	if(err == -1){
		printf("Ha habido un error al leer el archivo de rutas.\n");
		return -1;
	}
	//Inicializamos la tabla caché de ARP:
	err = arp_open();
	if(err == -1){
		printf("Ha habido un error al abrir ARP.\n");
		return -1;
	}
	return 0;
}

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

int ipv4_close()
{
	int err;
	err=eth_close(my_iface);
	if(err == -1){
		printf("Ha habido un error al cerrar la interfaz.\n");
		return -1;
	}
	ipv4_route_table_free(my_table);
	err = arp_close();
	if(err == -1){
		printf("Ha habido un error al cerrar ARP.\n");
		return -1;
	}
	
	return 0;
}

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

void calculate_ipv4_broadcast (ipv4_addr_t addr, ipv4_addr_t subnet, ipv4_addr_t* res_bcast)
{
	if (addr == NULL || subnet == NULL){
		res_bcast = NULL;
		return;
	}
	int i;
	unsigned char byte[IPv4_ADDR_SIZE];

	for(i=0; i < IPv4_ADDR_SIZE; i++){
		byte[i] = ~(subnet[i]);//Complemento a 1 para convertir 0 a 1.
		byte[i] |= addr[i];//Mascara OR para el calculo.
	}
	
	memcpy( res_bcast, &byte, IPv4_ADDR_SIZE);	
}

/* uint16_t ipv4_checksum ( unsigned char * data, int len )
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
uint16_t ipv4_checksum ( unsigned char * data, int len )
{
  int i;
  uint16_t word16;
  unsigned int sum = 0;
    
  /* Make 16 bit words out of every two adjacent 8 bit words in the packet
   * and add them up */
  for (i=0; i<len; i=i+2) {
    word16 = ((data[i] << 8) & 0xFF00) + (data[i+1] & 0x00FF);
    sum = sum + (unsigned int) word16;	
  }

  /* Take only 16 bits out of the 32 bit sum and add up the carries */
  while (sum >> 16) {
    sum = (sum & 0xFFFF) + (sum >> 16);
  }

  /* One's complement the result */
  sum = ~sum;

  return (uint16_t) sum;
}


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
void ipv4_addr_str ( ipv4_addr_t addr, char* str )
{
  if (str != NULL) {
    sprintf(str, "%d.%d.%d.%d",
            addr[0], addr[1], addr[2], addr[3]);
  }
}

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
int ipv4_str_addr ( char* str, ipv4_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[IPv4_ADDR_SIZE];
    int len = sscanf(str, "%d.%d.%d.%d", 
                     &addr_int[0], &addr_int[1], 
                     &addr_int[2], &addr_int[3]);

    if (len == IPv4_ADDR_SIZE) {
      int i;
      for (i=0; i<IPv4_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }
      
      err = 0;
    }
  }
  
  return err;
}

/* int ipv4_recv
 *	(uint8_t layer_prot, ipv4_addr_t ipv4_addr_src, unsigned char * buff_out , long int timeout);
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener el siguiente paquete recibido por la
 *   interfaz Ethernet indicada. La operación puede esperar indefinidamente o
 *   un tiempo limitando dependiento del parámetro 'timeout'. Recive tambien los
 *	 paquetes enviados a su dirección de broadcast. Realiza comprobación de checksum.
 *
 *   Esta función sólo permite recibir paquetes de una única interfaz. Si desea
 *   escuchar varias interfaces Ethernet simultaneamente, utilice la función
 *   'eth_poll()'.
 *
 * PARÁMETROS:
 *    'layer_prot': Indica el tipo de protocolo que queremos que tenga el paquete a recibir.
 * 		
 *	  'ipv4_addr_src': Dirección IPv4 donde se guarda la dirección de origen del paquete recivido.
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
(uint8_t layer_prot, ipv4_addr_t ipv4_addr_src, unsigned char * buff_out , long int timeout)
{
	int err;
	mac_addr_t mac_src;
	timerms_t timer;
	timerms_reset(&timer, timeout);
	long int time_left;
	unsigned char buffer[ETH_MTU];
	ipv4_header_t* ip_packet = NULL;
	uint16_t checksum_recv, checksum;

	do{
		time_left=timerms_left(&timer);
		//Recibimos de ethernet por nuestra interfaz y guardamos el contenido del paquete en buffer
		err=eth_recv(my_iface, mac_src, IPv4_PROT_TYPE,(void*) buffer, time_left);
		ip_packet = (ipv4_header_t*) buffer;

		if(err >= IPv4_HEADER_SIZE){ //Esperamos recibir un paquete IP.
			checksum_recv = ntohs(ip_packet->checksum);//Obtenemos el checksum del paquete.
			if( checksum_recv != 0){
				ip_packet->checksum = 0;
				checksum = ipv4_checksum((void*) ip_packet, IPv4_HEADER_SIZE);
			}
			if( checksum == checksum_recv || checksum_recv == 0){ //El checksum puede ser opcional.
#ifdef DEBUG
				printf("IPv4 Checksum ok!\n");
#endif
				if
				( (ip_packet->layer_prot == layer_prot)
				&& (memcmp(ip_packet->dest_ip_addr, my_ip, IPv4_ADDR_SIZE) == 0
				|| memcmp(ip_packet->dest_ip_addr, ip_bcast, IPv4_ADDR_SIZE) == 0) 
				&& ((((ip_packet->info_packet) &0xF0) >> 4) == IPv4_VERSION) )
				{
#ifdef DEBUG
					ip_packet->checksum = checksum_recv;
					printf(" Paquete IPv4 recibido: \n");
					print_pkt_ipv4(ip_packet);
#endif
				//Guardamos en buff_out el contenido del paquete IP:
					memcpy(buff_out, ip_packet -> data, (ntohs(ip_packet->packet_len) - IPv4_HEADER_SIZE));
					memcpy(ipv4_addr_src, ip_packet -> src_ip_addr, IPv4_ADDR_SIZE);
					//Devolvemos el numero de bytes del payload del paquete ip recibido.
					return (ntohs(ip_packet->packet_len) - IPv4_HEADER_SIZE);
				}
			}
			else{
				printf("Paquete corrupto.\n");
				err = -1;
				return err;
			}

		}
		if(err == -1){
			printf("Ha habido un error al recibir el paquete.\n");
			return -1;
		}
	}
	while(time_left > 0 || time_left < -1 );
	
	printf("Ha expirado el tiempo de recibir el paquete.\n");
	return -1;
}

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
(ipv4_addr_t ip_addr_dest, uint8_t layer_prot, char* payload, int payload_len)
{
	if ( payload_len > IPv4_PAYLOAD_LEN ){
		printf("Payload demasiado grande.\n");
		printf("Esta versión de IPv4 no permite fragmentacion... Por ahora.\n");
		return -1;
	}
	mac_addr_t mac_addr_dest;
	ipv4_route_t* ruta_optima;
	int err;
	ipv4_header_t ip_packet;
	ipv4_addr_t next_hop;
	ipv4_addr_t su_subred_ip;
	char * su_subred = "0.0.0.0";
	ipv4_str_addr ( su_subred, su_subred_ip );	
	
	/* Rellenamos la estructura. */
	ip_packet.info_packet = (IPv4_VERSION << 4) + (IPv4_HEADER_SIZE/4); //Corremos medio byte. 
	ip_packet.type_service = 0;
	ip_packet.packet_len = htons( IPv4_HEADER_SIZE + payload_len );
	ip_packet.id= htons(0); //No es necesario htons().
	ip_packet.flag_offset = htons(0x4000); //Don't frag
	ip_packet.ttl = DEFAULT_TTL;
	ip_packet.layer_prot = layer_prot;
	ip_packet.checksum = 0x0000;
	memcpy(ip_packet.src_ip_addr, my_ip, sizeof(ipv4_addr_t));
	memcpy(ip_packet.dest_ip_addr,  ip_addr_dest, sizeof(ipv4_addr_t));
	memcpy(ip_packet.data, payload, payload_len);
	ip_packet.checksum = htons(ipv4_checksum((void*)&ip_packet, IPv4_HEADER_SIZE));//Checksum de la cabecera
	
	ruta_optima=ipv4_route_table_lookup(my_table, ip_addr_dest);//La ruta tiene que estar inicializada.
	
	if(ruta_optima == NULL){
		printf("Destino inalcanzable.\n");
		return -1;
	}

	memcpy(next_hop,ruta_optima->next_hop_addr, IPv4_ADDR_SIZE);

	if( memcmp(next_hop, su_subred_ip, IPv4_ADDR_SIZE) == 0)
		memcpy(ruta_optima->next_hop_addr, ip_addr_dest, IPv4_ADDR_SIZE);

	if(memcmp(ip_addr_dest, ip_bcast, IPv4_ADDR_SIZE) == 0){
		memcpy(mac_addr_dest, BROADCAST_MAC_ADDR, MAC_ADDR_SIZE);
	}
	else{
		err = arp_resolve( my_iface, ruta_optima->next_hop_addr, mac_addr_dest); //Resolvemos el siguiente salto de nuestra IP.
		if(err == -1){
			printf("No se ha podido resolver la direccion IPv4.\n");
			return -1;
		}
	}
#ifdef DEBUG
	printf(" Paquete IPv4 enviado: \n");
	print_pkt_ipv4(&ip_packet);
#endif
	err = eth_send( my_iface, mac_addr_dest, IPv4_PROT_TYPE, (void*)&ip_packet, IPv4_HEADER_SIZE + payload_len);

	if (err == -1){
		printf("Ha habido un error al enviar el datagrama IP.\n");
		return -1;
	}

	return err;
}
