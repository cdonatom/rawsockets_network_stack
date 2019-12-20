#include "ripv2.h"
#include "../udp/udp.h"
#include "../ipv4/ipv4_route_table.h"
#include "ripv2_route_table.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <timerms.h>
#include <arpa/inet.h>

#define TIMEOUT_TIMER_RIP 180000
#define NO_TIMEOUT -1
#define UPDATE_RIP 30000
#define GARBAGE_COLLECTION 120000

int main ( int argc, char * argv[] ){
	if (argc != 1) {
		printf("Uso: %s \n", argv[0]); 
		exit(-1);
	}
	int err, packet_size;
	char* filename_conf="ipv4/config/ipv4_config_server.txt";
	char* filename_rutas="ipv4/config/ipv4_route_table_server.txt";

	//Inicializamos UDP con el puerto de RIP:
	err = udp_open(filename_conf, filename_rutas, RIP_PORT);
	if (err == -1){
		printf("Ha habido un error al abrir UDP\n");
		exit(-1);
	}
	//Direcciones IPv4 que vamos a necesitar.
	ipv4_addr_t ipv4_addr, ipv4_addr_bcast;
	get_my_ip_bcast(ipv4_addr_bcast);
	//Paquete temporal de RIPv2.
	rip_packet_t rip_packet;
	memset ((void*) &rip_packet, 0, sizeof(rip_packet_t));
	//Inicializamos un timer para el envio periódico de Updates.
	timerms_t timer_update;
	timerms_reset(&timer_update, UPDATE_RIP);

	//Tablas de rutas de IPv4.
	ipv4_route_table_t * ipv4_table = get_my_table();
	ipv4_route_t* ipv4_route;
	char* iface = get_my_iface();
	//Tablas de rutas de RIPv2.
	ripv2_route_table_t* rip_table = ripv2_route_table_create();
	ripv2_route_t * rip_route_ptr;
	long int min_time = timerms_left(&timer_update);
	uint16_t socket_rcv;

	int i=0;
	int j=0;
	int limit;
	int ipv4_index;
	//Convertimos la tabla de rutas de IPv4 a RIPv2.
	for (i = 0; i < IPv4_ROUTE_TABLE_SIZE; i++){
		ipv4_route = ipv4_route_table_get ( ipv4_table, i);
		if ( ipv4_route != NULL){
			rip_route_ptr = ripv2_route_create (ipv4_route->subnet_addr,
				ipv4_route->subnet_mask, ipv4_route->iface, EMPTY_IPv4, 1, NO_TIMEOUT);//Metrica 1
			err = ripv2_route_table_add (rip_table, rip_route_ptr);
			if (err == -1){
				printf("Ha habido un error al insertar las rutas IPv4 en la tabla RIP.\n");
			}
		}
	}
	//Comenzamos a escuchar durante 30 segundos:
	while(1){
		
		packet_size = udp_recv(&socket_rcv, ipv4_addr, (void*) &rip_packet, min_time );
		//Si el paquete Recibido es de tipo REQUEST:
		if( rip_packet.command == REQUEST ){
			//Creamos el paquete RESPONSE...
			packet_size = create_response ( &rip_packet, rip_table);
			//Y lo enviamos por UDP a la ipv4 de la que procedía el REQUEST:
			err = udp_send (ipv4_addr, RIP_PORT, (void*)&rip_packet, packet_size);
			memset ((void*) &rip_packet, 0, sizeof(rip_packet_t));
		}
		//Si el paquete Recibido es de tipo RESPONSE:
		if( rip_packet.command == RESPONSE ){
			limit = (packet_size - RIP_PACKET_HEADER)/sizeof(rip_entry_t);//Número de entradas.
			//Bucle que recorre la entradas del paquete:
			for( i = 0; i < limit; i++ ){
				//Bucle que recorre las entradas de la tabla de rutas:
			    for (j = 0; j < RIPv2_ROUTE_TABLE_SIZE; j++ ){
					rip_route_ptr = ripv2_route_table_get (rip_table, j);
					if (rip_route_ptr != NULL ){
						//Comprobamos si las entradas son iguales:
						if ( memcmp(rip_route_ptr->subnet_addr, 
							&(rip_packet.list_entry[i].ipv4_addr), IPv4_ADDR_SIZE ) == 0 )
						{
#ifdef DEBUG
						    printf("RIP: Rutas iguales!\n");
#endif
							//Comprobamos si la métrica es menor o igual:
						    if( (htonl(rip_packet.list_entry[i].metric)+1) <= rip_route_ptr->metric ){ 
#ifdef DEBUG
						    printf("RIP: Actualizando rutas!\n");
#endif
								ripv2_route_table_remove( rip_table, j );//Borramos esa entrada
								//Creamos y añadimos la entrada actualizada.
								rip_route_ptr = ripv2_route_create( rip_packet.list_entry[i].ipv4_addr,
									rip_packet.list_entry[i].subnet_mask, iface, 
									rip_packet.list_entry[i].next_hop, htonl(rip_packet.list_entry[i].metric) + 1,
									TIMEOUT_TIMER_RIP);
								err = ripv2_route_table_add (rip_table, rip_route_ptr);
								if (err == -1){
									printf("Ha habido un error al insertar las rutas IPv4 en la tabla RIP.\n");
									break;
								}
								//SINCRONIZACION CON IP:
								ipv4_route = ipv4_route_create 
									( rip_packet.list_entry[i].ipv4_addr, rip_packet.list_entry[i].subnet_mask,
										iface, ipv4_addr);
								err = ipv4_route_table_add ( ipv4_table, ipv4_route);
								if (err == -1){
									printf("Ha habido un error al sincronizar la tabla RIP con la IPv4.\n");
									break;
								}
						    }
						break;
						}
					}
					/*else{
#ifdef DEBUG
					    printf("RIP: Estoy creando una ruta!\n");
#endif
					    rip_route_ptr = ripv2_route_create( rip_packet.list_entry[i].ipv4_addr,
								rip_packet.list_entry[i].subnet_mask, iface, 
								rip_packet.list_entry[i].next_hop, htonl(rip_packet.list_entry[i].metric) + 1,
								TIMEOUT_TIMER_RIP );
#ifdef DEBUG
					    printf("RIP: Estoy aniadiendo una ruta!\n");
#endif
						err = ripv2_route_table_add (rip_table, rip_route_ptr);
						if (err == -1){
							printf("Ha habido un error al insertar las rutas IPv4 en la tabla RIP.\n");
							break;
						}
						ipv4_route = ipv4_route_create 
								( rip_packet.list_entry[i].ipv4_addr, rip_packet.list_entry[i].subnet_mask,
									iface, ipv4_addr);
						err = ipv4_route_table_add ( ipv4_table, ipv4_route);
						if (err == -1){
							printf("Ha habido un error al sincronizar la tabla RIP con la IPv4.\n");
							break;
						}
						break;
					}*/
				}
				//Si ha recorrido toda la tabla y no ha encontrado una ruta igual...
				if ( j == RIPv2_ROUTE_TABLE_SIZE ){
#ifdef DEBUG
				    printf("RIP: Estoy aniadiendo una ruta!\n");
#endif
				    rip_route_ptr = ripv2_route_create( rip_packet.list_entry[i].ipv4_addr,
								rip_packet.list_entry[i].subnet_mask, iface, 
								rip_packet.list_entry[i].next_hop, htonl(rip_packet.list_entry[i].metric) + 1,
								TIMEOUT_TIMER_RIP );
					err = ripv2_route_table_add (rip_table, rip_route_ptr);
					if (err == -1){
						printf("Ha habido un error al insertar las rutas IPv4 en la tabla RIP.\n");
						break;
					}
					//SINCRONIZACION CON IP:
					ipv4_route = ipv4_route_create 
						( rip_packet.list_entry[i].ipv4_addr, rip_packet.list_entry[i].subnet_mask,
							iface, ipv4_addr);
					err = ipv4_route_table_add ( ipv4_table, ipv4_route);
					if (err == -1){
						printf("Ha habido un error al sincronizar la tabla RIP con la IPv4.\n");
						break;
					}
				}
			}
			memset ((void*) &rip_packet, 0, sizeof(rip_packet_t));
		}
		
		if( timerms_left(&timer_update) == 0 ){ // Anunciamos nuestras rutas cada 30 segundos.
			timerms_reset(&timer_update, UPDATE_RIP);
			packet_size = create_response ( &rip_packet, rip_table );
			err = udp_send (ipv4_addr_bcast, RIP_PORT, (void*)&rip_packet, packet_size);// Anunciamos por bcast.
			memset ((void*) &rip_packet, 0, sizeof(rip_packet_t));
		}
		if ( err == -1 ){
			printf("Ha habido un error al enviar el paquete RIP.\n");
			break; //Salimos del bucle para liberar la memoria y cerrar UDP.
		}
		//Hacemos un repaso a la tabla de rutas:
		for(i = 0; i < IPv4_ROUTE_TABLE_SIZE; i++){
			rip_route_ptr = ripv2_route_table_get ( rip_table, i);
			//Si el timer de la ruta ha finalizado...
			if( rip_route_ptr != NULL && timerms_left(&(rip_route_ptr -> timer)) == 0 ){
				//Si ya tiene metrica infinito la borramos
				if( rip_route_ptr->metric == INFINITY_METRIC){
					ipv4_index = ipv4_route_table_find 
						(ipv4_table, rip_route_ptr->subnet_addr, rip_route_ptr->subnet_mask);
					if (ipv4_index >= 0)
						ipv4_route_table_remove(ipv4_table, ipv4_index);
					ripv2_route_table_remove ( rip_table, i);//En principio habría que ponerla a infinito.
				}
				//Si no tiene metrica infinito, se la ponemos, y actualizamos su timer 120s.
				else{
					timerms_reset(&(rip_route_ptr -> timer), GARBAGE_COLLECTION);
					rip_route_ptr->metric = INFINITY_METRIC;
				}
			}
		}
		if (packet_size > 0){
			memset ((void*) &rip_packet, 0, sizeof(rip_packet_t));
		}
		//Volvemos a recorrer la tabla, esta vez para actualizar el tiempo de escucha al menor de los timers de las entradas.
		for (i = 0; i < RIPv2_ROUTE_TABLE_SIZE; i++){
			rip_route_ptr = ripv2_route_table_get (rip_table, i);
			if (rip_route_ptr != NULL ){
				if ( min_time > timerms_left(&(rip_route_ptr->timer)) 
					&& timerms_left(&(rip_route_ptr->timer)) > 0){
					min_time = timerms_left(&(rip_route_ptr->timer));
				}
				if ( min_time > timerms_left(&timer_update)){
					min_time = timerms_left(&(rip_route_ptr->timer));
				}
			}
		}
	}

	ripv2_route_table_free(rip_table);
	udp_close();
	return 0;
}