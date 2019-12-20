#include "ripv2_route_table.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define  TIMEOUT_TIMER_RIP 180000
struct ripv2_route_table {
  ripv2_route_t * entrys[RIPv2_ROUTE_TABLE_SIZE];
};

/* ripv2_route_t * ripv2_route_create
 *	( ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t nhop, uint32_t metric, long int timeout);
 * 
 * DESCRIPCIÓN: 
 *   Esta función crea una ruta Ripv2 con los parámetros especificados:
 *   dirección de subred, máscara, nombre de interfaz , dirección de siguiente
 *   salto, métrica y timer.
 *   Esta función reserva memoria para la estructura creada.
 *
 * PARÁMETROS:
 *   'subnet': Dirección IPv4 de la subred destino de la nueva ruta.
 *     'mask': Máscara de la subred destino de la nueva ruta.
 *    'iface': Nombre del interfaz empleado para llegar a la subred destino de
 *             la nueva  ruta.
 *             Debe tener una longitud máxima de 'IFACE_NAME_LENGTH' caracteres.
 *     'nhop': Dirección IPv4 del siguiente salto empleado para llegar a la
 *             subred destino de la nueva ruta.
 *	 'metric': Distancia hasta el destino.
 *	'timeout': Tiempo de vida de la ruta.
 *
 * VALOR DEVUELTO:
 *   Devuelve la entrada RIP creada.
 * 
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la ruta.
 */
ripv2_route_t * ripv2_route_create
( ipv4_addr_t subnet, ipv4_addr_t mask, char* iface, ipv4_addr_t nhop, uint32_t metric, long int timeout)
{
  ripv2_route_t * entry = (ripv2_route_t *) malloc(sizeof(struct ripv2_route));
  if (entry != NULL) {   
	memcpy(entry->subnet_addr, subnet, IPv4_ADDR_SIZE);
    memcpy(entry->subnet_mask, mask, IPv4_ADDR_SIZE);
    strcpy(entry->iface, iface);
    memcpy(entry->next_hop_addr, nhop, IPv4_ADDR_SIZE);
	entry->metric = metric;
	if (timeout == -1){
		timerms_reset(&(entry->timer), timeout);
	}
	else{
		timerms_reset(&(entry->timer), TIMEOUT_TIMER_RIP);//Ponemos un timer de 180 segundos a la ruta
	}
  }
  
  return entry;
}

/* int ripv2_route_lookup ( ripv2_route_t * entry, ipv4_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función indica si la dirección IPv4 especificada pertence a la
 *   subred indicada. En ese caso devuelve la longitud de la máscara de la
 *   subred.
 *
 *   Esta función NO está implementada, debe implementarla usted para que
 *   funcione correctamente la función 'ripv2_route_table_lookup()'.
 * 
 * PARÁMETROS:
 *   'entry': Ruta a la subred que se quiere comprobar.
 *    'addr': Dirección IPv4 destino.
 *
 * VALOR DEVUELTO:
 *   Si la dirección IPv4 pertenece a la subred de la ruta especificada, debe
 *   devolver un número positivo que indica la longitud del prefijo de
 *   subred. Esto es, el número de bits a uno de la máscara de subred.
 *   La función devuelve '-1' si la dirección IPv4 no pertenece a la subred
 *   apuntada por la ruta especificada.
 */

int ripv2_route_lookup ( ripv2_route_t * entry, ipv4_addr_t addr ){
	int prefix_length = -1;
	int i;
	ipv4_addr_t ip,ip2;
	ipv4_addr_t mascara;
	ipv4_addr_t subnet;
	
	memcpy(mascara, entry->subnet_mask, IPv4_ADDR_SIZE);
	memcpy(subnet, entry->subnet_addr, IPv4_ADDR_SIZE);

	for (i=0;i<IPv4_ADDR_SIZE;i++){
		ip[i] = ( addr[i] & mascara[i]);
		ip2[i] = ( subnet[i] & mascara[i]);
	}

	if(memcmp(ip,ip2,IPv4_ADDR_SIZE)==0){
		i=0;
		int tam=0;
		
		for(i=0; i<4; i++){
			if(mascara[i] == 0xff){
				tam+=8;
			} else {
				switch(mascara[i]){
					case 0x00:
						break;
					case 0x80:
						tam+=1;
						break;
					case 0xc0:
						tam+=2;
						break;
					case 0xe0:
						tam+=3;
						break;
					case 0xf0:
						tam+=4;
						break;		
					case 0xf8:
						tam+=5;
						break;			
					case 0xfc:
						tam+=6;
						break;		
					case 0xfe:
						tam+=7;
						break;
				} 			
			}
		}
		prefix_length=tam;
	}
	return prefix_length;
}



/*
int ripv2_route_lookup ( ripv2_route_t * entry, ipv4_addr_t addr )
{
	int prefix_length = -1;
	int i;
	ipv4_addr_t ip,ip2;
	ipv4_addr_t mascara;
	ipv4_addr_t subnet;
	
	memcpy(mascara, entry->subnet_mask, IPv4_ADDR_SIZE);
	memcpy(subnet, entry->subnet_addr, IPv4_ADDR_SIZE);

	for (i=0;i<IPv4_ADDR_SIZE;i++){
		ip[i] = ( addr[i] & mascara[i]);
		ip2[i] = ( subnet[i] & mascara[i]);
	}

	
	if(memcmp(ip,ip2,IPv4_ADDR_SIZE)==0){
		i=3;
		int tam=32;
		while (i>-1){
			if (mascara[i] != 0x00 ){
			
				switch(mascara[i]){

				case 0xff:
					prefix_length = tam;
					break;
				case 0xfe:
					prefix_length = tam-1;
					break;
				case 0xfc:
					prefix_length = tam-2;
					break;
				case 0xf8:
					prefix_length = tam-3;
					break;
				case 0xf0:
					prefix_length = tam-4;
					break;
				case 0xe0:
					prefix_length = tam-5;
					break;
				case 0xc0:
					prefix_length = tam-6;
					break;
				case 0x80:
					prefix_length = tam-7;
					break;

				}

			}
			if (prefix_length != -1)
				break;
			tam=tam-8;
			i--;			
		}

	}

	return prefix_length;
}
*/
/* void ripv2_route_print ( ripv2_route_t * entry );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime la ruta especificada por la salida estándar.
 *
 * PARÁMETROS:
 *   'entry': Ruta que se desea imprimir.
 */
void ripv2_route_print ( ripv2_route_t * entry )
{
  if (entry != NULL) {
    char subnet_str[IPv4_ADDR_STR_LENGTH];
    ipv4_addr_str(entry->subnet_addr, subnet_str);
    char mask_str[IPv4_ADDR_STR_LENGTH];
    ipv4_addr_str(entry->subnet_mask, mask_str);
    char* iface_str = entry->iface;
    char nhop_str[IPv4_ADDR_STR_LENGTH];
    ipv4_addr_str(entry->next_hop_addr, nhop_str);
	
    printf("%s/%s -> %s/%s %i %li\n", subnet_str, mask_str, iface_str, nhop_str,
		entry->metric, timerms_left(&(entry->timer)) );
  }
}


/* void ripv2_route_free ( ripv2_route_t * entry );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la ruta especificada, que
 *   ha sido creada con 'ripv2_route_create()'.
 *
 * PARÁMETROS:
 *   'entry': Ruta que se desea liberar.
 */
void ripv2_route_free ( ripv2_route_t * entry )
{
  if (entry != NULL) {
    free(entry);
  }
}



/* ripv2_route_table_t * ripv2_route_table_create();
 * 
 * DESCRIPCIÓN: 
 *   Esta función crea una tabla de rutas ripv2 con las entradas a NULL.
 *
 * VALOR DEVUELTO:
 *   La función devuelve un puntero a la tabla de rutas creada.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si no ha sido posible reservar memoria para
 *   crear la tabla de rutas.
 */
ripv2_route_table_t * ripv2_route_table_create()
{
  ripv2_route_table_t * table;

  table = (ripv2_route_table_t *) malloc(sizeof(struct ripv2_route_table));
  if (table != NULL) {
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      table->entrys[i] = NULL;
    }
  }

  return table;
}


/* int ripv2_route_table_add ( ripv2_route_table_t * table, 
 *                            ripv2_route_t * entry );
 * DESCRIPCIÓN: 
 *   Esta función añade la ruta especificada en la primera posición libre de
 *   la tabla de rutas.
 *
 * PARÁMETROS:
 *   'table': Tabla donde añadir la ruta especificada.
 *   'entry': Ruta a añadir en la tabla de rutas.
 * 
 * VALOR DEVUELTO:
 *   La función devuelve el indice de la posición [0,RIPv2_ROUTE_TABLE_SIZE-1]
 *   donde se ha añadido la ruta especificada.
 * 
 * ERRORES:
 *   La función devuelve '-1' si no ha sido posible añadir la ruta
 *   especificada.
 */
int ripv2_route_table_add ( ripv2_route_table_t * table, ripv2_route_t * entry )
{
  int entry_index = -1;

  if (table != NULL) {
    /* Find an empty place in the entry table */
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      if (table->entrys[i] == NULL) {
        table->entrys[i] = entry;
        entry_index = i;
        break;
      }
    }
  }

  return entry_index;
}


/* ripv2_route_t * ripv2_route_table_remove ( ripv2_route_table_t * table, 
 *                                          int index );
 *
 * DESCRIPCIÓN:
 *   Esta función borra la ruta almacenada en la posición de la tabla de rutas
 *   especificada.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea borrar una ruta.
 *   'index': Índice de la ruta a borrar. Debe tener un valor comprendido
 *            entre [0, RIPv2_ROUTE_TABLE_SIZE-1].
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta que estaba almacenada en la posición
 *   indicada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si la ruta no ha podido ser borrada, o no
 *   existía ninguna ruta en dicha posición.
 */
ripv2_route_t * ripv2_route_table_remove ( ripv2_route_table_t * table, int index )
{
  ripv2_route_t * removed_entry = NULL;
  
  if ((table != NULL) && (index >= 0) && (index < RIPv2_ROUTE_TABLE_SIZE)) {
    removed_entry = table->entrys[index];
    table->entrys[index] = NULL;
  }

  return removed_entry;
}

/* ripv2_route_t * ripv2_route_table_get ( ripv2_route_table_t * table, int index );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve la ruta almacenada en la posición de la tabla de
 *   rutas especificada.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas de la que se desea obtener una ruta.
 *   'index': Índice de la ruta consultada. Debe tener un valor comprendido
 *            entre [0, RIPv2_ROUTE_TABLE_SIZE-1].
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta almacenada en la posición de la tabla de
 *   rutas indicada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si no ha sido posible consultar la tabla de
 *   rutas, o no existe ninguna ruta en dicha posición.
 */
ripv2_route_t * ripv2_route_table_get ( ripv2_route_table_t * table, int index )
{
  ripv2_route_t * entry = NULL;

  if ((table != NULL) && (index >= 0) && (index < RIPv2_ROUTE_TABLE_SIZE)) {
    entry = table->entrys[index];
  }
  
  return entry;
}

/* int ripv2_route_table_find ( ripv2_route_table_t * table, ipv4_addr_t subnet, 
 *                                                         ipv4_addr_t mask );
 *
 * DESCRIPCIÓN:
 *   Esta función devuelve el índice de la ruta para llegar a la subred
 *   especificada.
 *
 * PARÁMETROS:
 *    'table': Tabla de rutas en la que buscar la subred.
 *   'subnet': Dirección de la subred a buscar.
 *     'mask': Máscara de la subred a buscar.
 * 
 * VALOR DEVUELTO:
 *   Esta función devuelve la posición de la tabla de rutas donde se encuentra
 *   la ruta que apunta a la subred especificada.
 *
 * ERRORES:
 *   La función devuelve '-1' si no se ha encontrado la ruta especificada o
 *   '-2' si no ha sido posible realizar la búsqueda.
 */
int ripv2_route_table_find
( ripv2_route_table_t * table, ipv4_addr_t subnet, ipv4_addr_t mask )
{
  int entry_index = -2;

  if (table != NULL) {
    entry_index = -1;
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      ripv2_route_t * entry_i = table->entrys[i];
      if (entry_i != NULL) {
        int same_subnet = 
          (memcmp(entry_i->subnet_addr, subnet, IPv4_ADDR_SIZE) == 0);
        int same_mask = 
          (memcmp(entry_i->subnet_mask, mask, IPv4_ADDR_SIZE) == 0);
        
        if (same_subnet && same_mask) {
          entry_index = i;
          break;
        }
      }
    }
  }

  return entry_index;
}

/* ripv2_route_t * ripv2_route_table_lookup ( ripv2_route_table_t * table, 
 *                                          ipv4_addr_t addr );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve la mejor ruta almacenada en la tabla de rutas para
 *   alcanzar la dirección IPv4 destino especificada.
 *
 *   Esta función recorre toda la tabla de rutas buscando rutas que contengan
 *   a la dirección IPv4 indicada. Para ello emplea la función
 *   'ripv2_route_lookup()'. De todas las rutas posibles se devuelve aquella
 *   con el prefijo más específico, esto es, aquella con la máscara de subred
 *   mayor.
 * 
 * PARÁMETROS:
 *   'table': Tabla de rutas en la que buscar la dirección IPv4 destino.
 *    'addr': Dirección IPv4 destino a buscar.
 *
 * VALOR DEVUELTO:
 *   Esta función devuelve la ruta más específica para llegar a la dirección
 *   IPv4 indicada.
 *
 * ERRORES:
 *   Esta función devuelve 'NULL' si no no existe ninguna ruta para alcanzar
 *   la dirección indicada, o si no ha sido posible realizar la búsqueda.
 */
ripv2_route_t * ripv2_route_table_lookup ( ripv2_route_table_t * table, 
                                         ipv4_addr_t addr )
{
  ripv2_route_t * best_entry = NULL;
  int best_entry_prefix = -1;

  if (table != NULL) {
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      ripv2_route_t * entry_i = table->entrys[i];
      if (entry_i != NULL) {
        int entry_i_lookup = ripv2_route_lookup(entry_i, addr);
        if (entry_i_lookup > best_entry_prefix) {
          best_entry = entry_i;
          best_entry_prefix = entry_i_lookup;
        }
      }
    }
  }
  
  return best_entry;
}

/* void ripv2_route_table_free ( ripv2_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función libera la memoria reservada para la tabla de rutas
 *   especificada, incluyendo todas las rutas almacenadas en la misma,
 *   mediante la función 'ripv2_route_free()'.
 *
 * PARÁMETROS:
 *   'table': Tabla de rutas a borrar.
 */
void ripv2_route_table_free ( ripv2_route_table_t * table )
{
  if (table != NULL) {
    int i;
    for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
      ripv2_route_t * entry_i = table->entrys[i];
      if (entry_i != NULL) {
        table->entrys[i] = NULL;
        ripv2_route_free(entry_i);
      }
    }
    free(table);
  }
}



/* void ripv2_route_table_output ( ripv2_route_table_t * table, FILE * out );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida indicada la tabla de rutas IPv4
 *   especificada.
 *
 * PARÁMETROS:
 *        'out': Salida por la que imprimir la tabla de rutas.
 *      'table': Tabla de rutas a imprimir.
 *
 * VALOR DEVUELTO:
 *   La función devuelve el número de rutas impresas por la salida indicada, o
 *   '0' si la tabla de rutas estaba vacia.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error al escribir por
 *   la salida indicada.
 */
int ripv2_route_table_output ( FILE * out, ripv2_route_table_t * table )
{
  int err = fprintf
    (out, "# SubnetAddr  \tSubnetMask    \tIface  \tNextHop  \tMetric  \t Time_left\n");
  if (err < 0) {
    return -1;
  }
  
  char subnet_str[IPv4_ADDR_STR_LENGTH];
  char mask_str[IPv4_ADDR_STR_LENGTH];
  char* ifname = NULL;
  char nhop_str[IPv4_ADDR_STR_LENGTH];

  int i;
  for (i=0; i<RIPv2_ROUTE_TABLE_SIZE; i++) {
    ripv2_route_t * entry_i = ripv2_route_table_get(table, i);
    if (entry_i != NULL) {
      ipv4_addr_str(entry_i->subnet_addr, subnet_str);
      ipv4_addr_str(entry_i->subnet_mask, mask_str);
      ifname = entry_i->iface;
      ipv4_addr_str(entry_i->next_hop_addr, nhop_str);

      err = fprintf(out, "%-15s\t%-15s\t%s\t%s\t%i\t%li\n",
                    subnet_str, mask_str, ifname, nhop_str, 
					entry_i->metric, timerms_left(&(entry_i->timer)));
      if (err < 0) {
        return -1;
      }
    }
  }

  return 0;
}


/* void ripv2_route_table_print ( ripv2_route_table_t * table );
 *
 * DESCRIPCIÓN:
 *   Esta función imprime por la salida estándar la tabla de rutas IPv4
 *   especificada.
 *
 * PARÁMETROS:
 *      'table': Tabla de rutas a imprimir.
 */
void ripv2_route_table_print ( ripv2_route_table_t * table )
{
  if (table != NULL) {
    ripv2_route_table_output (stdout, table);
  }
}

