#include "eth.h"
#include <rawnet.h>
#include <timerms.h>

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netinet/in.h>

/* Información de depuración */
#include "debug.h"
#define ETH_LOG DEBUG_LOG_LEVEL
#define ETH_MIN_SIZE_PAYLOAD 46
/* Dirección MAC de difusión: FF:FF:FF:FF:FF:FF */
mac_addr_t BROADCAST_MAC_ADDR = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };

/* Dirección MAC vacía: 00:00:00:00:00:00 */
mac_addr_t EMPTY_MAC_ADDR = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

/* Estructura del manejador del interfaz ethernet */
struct eth_iface {
  rawiface_t * raw_iface; /* Manejador del interfaz "crudo" */
  mac_addr_t mac_address; /* Dirección MAC del interfaz. Se almacena aquí en
                             lugar de consultar al interfaz "en crudo" para
                             evitar una llamada al sistema adcional cada vez
                             que se quiera enviar una trama. */
};

/* Cabecera de una trama Ethernet */
struct eth_frame {
  mac_addr_t dest_addr; /* Dirección MAC destino*/
  mac_addr_t src_addr;  /* Dirección MAC origen */
  uint16_t type;        /* Campo 'Tipo'. 
                           Identificador de la capa de red superior */ 
  unsigned char payload[ETH_MTU]; /* Campo 'payload'. 
                                          Datos de la capa superior */

  /* NOTA: El campo "Frame Checksum" (FCS) no está incluido en la estructura
     porque lo añade automáticamente la tarjeta de red. */
};


/* eth_iface_t * eth_open ( char* ifname );
 *
 * DESCRIPCIÓN: 
 *   Esta función inicializa la interfaz Ethernet especificada para que pueda
 *   ser utilizada por las restantes funciones de la librería.
 *   La memoria del manejador de interfaz devuelto debe ser liberada con la
 *   función 'eth_close()'.
 *
 * PARÁMETROS:
 *   'ifname': Cadena de texto con el nombre de la interfaz Ethernet que se
 *             desea inicializar.
 *
 * VALOR DEVUELTO:
 *   Manejador de la interfaz Ethernet inicializada.
 *   Dicho manejador es un puntero a una estructura opaca que no debe ser
 *   accedida directamente, en su lugar use las funciones de la librería.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si se ha producido algún error. 
 */
eth_iface_t * eth_open ( char* ifname )
{
  struct eth_iface * eth_iface;

  /* Reservar memoria para el manejador del interfaz Ethernet */
  eth_iface = malloc(sizeof(struct eth_iface));
  if (eth_iface == NULL) {
    fprintf(stderr, "eth_open(): ERROR en malloc()\n");
    return NULL;
  }
  
  /* Abrir el interfaz "en crudo" subyacente */
  rawiface_t * raw_iface = rawiface_open(ifname);
  if (raw_iface == NULL) {
    fprintf(stderr, "eth_open(): ERROR en rawiface_open(): %s\n",
            rawnet_strerror());
    return NULL;
  }  
  eth_iface->raw_iface = raw_iface;

  /* Copiar la dirección MAC en el manejador */
  rawiface_getaddr(raw_iface, eth_iface->mac_address);

  /* DEBUG */
  if (ETH_LOG >= DEBUG_LOG_LEVEL) {
    printf("eth_open(%s)\n", ifname);
  }

  return eth_iface;
}


/* char * eth_getname ( eth_iface_t * iface );
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve el nombre de la interfaz Ethernet especificada.
 *  
 * PARÁMETROS:
 *   'iface': Manejador de la interfaz ethernet de la que se quiere obtener su
 *            nombre. 
 *            La interfaz debe haber sido inicializada con 'eth_open()'
 *            previamente.
 *
 * VALOR DEVUELTO:
 *   Cadena de texto con el nombre de la interfaz.
 *
 * ERRORES:
 *   La función devuelve 'NULL' si la interfaz no ha sido inicializada
 *   correctamente.
 */
char * eth_getname ( eth_iface_t * iface )
{
  char* iface_name = NULL;
  
  if (iface != NULL) {
    iface_name = rawiface_getname(iface->raw_iface);
  }

  return iface_name;
}

/* void eth_getaddr ( eth_iface_t * iface, mac_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener la dirección MAC de la interfaz Ethernet
 *   especificada.
 *
 * PARÁMETROS:
 *   'iface': Manejador de la interfaz Ethernet de la que se quiere obtener su
 *            dirección. 
 *            La interfaz debe haber sido inicializada con 'eth_open()'
 *            previamente.
 *    'addr': Array donde se copiará la dirección MAC de la interfaz Ethernet.
 *            Las direcciones MAC ocupan 'MAC_ADDR_SIZE' bytes.
 */
void eth_getaddr ( eth_iface_t * iface, mac_addr_t addr )
{
  if (iface != NULL) {
    memcpy(addr, iface->mac_address, MAC_ADDR_SIZE);
  }
}

/* int eth_send 
 * ( eth_iface_t * iface, 
 *   mac_addr_t dst, uint16_t type, unsigned char * payload, int payload_len )
 *
 * DESCRIPCIÓN:
 *   Esta función permite enviar una trama Ethernet a través de la interfaz
 *   indicada.
 *
 * PARÁMETROS:
 *    'iface': Manejador de la interfaz Ethernet por la que se quiere enviar
 *             el paquete.
 *             La interfaz debe haber sido inicializada con 'eth_open()'
 *             previamente.
 *      'dst': Dirección MAC del equipo destino.
 *     'type': Valor del campo 'Tipo' de la trama Ethernet a enviar.
 *     'payload': Puntero a los datos que se quieren enviar en la trama
 *             Ethernet.
 * 'payload_len': Longitud en bytes de los datos a enviar.
 *
 * VALOR DEVUELTO:
 *   El número de bytes que han podido ser enviados.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int eth_send 
( eth_iface_t * iface, 
  mac_addr_t dst, uint16_t type, unsigned char * payload, int payload_len )
{
  int bytes_sent;

  /* Comprobar parámetros */
  if (iface == NULL) {
    fprintf(stderr, "eth_send(): ERROR: iface == NULL\n");
    return -1;
  }

  /* Crear la Trama Ethernet y rellenar todos los campos */
  struct eth_frame eth_frame;
  memcpy(eth_frame.dest_addr, dst, MAC_ADDR_SIZE);
  memcpy(eth_frame.src_addr, iface->mac_address, MAC_ADDR_SIZE);
  eth_frame.type = htons(type);
  memset(eth_frame.payload, 0, ETH_MIN_SIZE_PAYLOAD);
  memcpy(eth_frame.payload, payload, payload_len);
  int eth_frame_len;
  if (payload_len < ETH_MIN_SIZE_PAYLOAD){
	payload_len = ETH_MIN_SIZE_PAYLOAD;
  }
  eth_frame_len = ETH_HEADER_SIZE + payload_len;

  /* DEBUG */
  if (ETH_LOG >= VERBOSE_LOG_LEVEL) {
    char* iface_name = eth_getname(iface);
    char mac_addr_str[MAC_ADDR_STR_LENGTH];
    eth_mac_str(dst, mac_addr_str);
    printf("eth_send(type=0x%04x, payload[%d]) > %s/%s\n",
           type, payload_len, iface_name, mac_addr_str);
    if (ETH_LOG >= DEBUG_LOG_LEVEL) {
      print_pkt((unsigned char *) &eth_frame, eth_frame_len, ETH_HEADER_SIZE);
    }
  }

  /* Enviar la trama Ethernet creada con rawnet_send() y comprobar errores */
  bytes_sent = rawnet_send
    (iface->raw_iface, (unsigned char *) &eth_frame, eth_frame_len);
  if (bytes_sent == -1) {
    fprintf(stderr, "eth_send(): ERROR en rawnet_send(): %s\n", 
            rawnet_strerror());
    return -1;
  }
  /* Devolver el número de bytes de datos enviados */
  return (bytes_sent - ETH_HEADER_SIZE);
}

/* int eth_recv 
 * ( eth_iface_t * iface, 
 *   mac_addr_t src, uint16_t type, unsigned char buffer[], long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite obtener el siguiente paquete recibido por la
 *   interfaz Ethernet indicada. La operación puede esperar indefinidamente o
 *   un tiempo limitando dependiento del parámetro 'timeout'.
 *
 *   Esta función sólo permite recibir paquetes de una única interfaz. Si desea
 *   escuchar varias interfaces Ethernet simultaneamente, utilice la función
 *   'eth_poll()'.
 *
 * PARÁMETROS:
 *    'iface': Manejador de la interfaz Ethernet por la que se desea recibir
 *             un paquete. 
 *             La interfaz debe haber sido inicializada con 'eth_open()'
 *             previamente.
 *      'src': Dirección MAC del equipo que envió la trama Ethernet recibida.
 *             Este es un parámetro de salida. La dirección se copiará en la
 *             memoria indicada, que debe estar reservada previamente.
 *     'type': Valor del campo 'Tipo' de la trama Ethernet que se desea
 *             recibir. 
 *             Las tramas con un valor 'type' diferente serán descartadas.
 *   'buffer': Array donde se almacenarán los datos de la trama recibida.
 *             Deben reservarse al menos 'ETH_MTU' bytes para recibir dichos
 *             datos.
 *  'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
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
int eth_recv 
( eth_iface_t * iface, 
  mac_addr_t src, uint16_t type, unsigned char buffer[], long int timeout)
{
  int payload_len;

  /* Comprobar parámetros */
  if (iface == NULL) {
    fprintf(stderr, "eth_recv(): ERROR: iface == NULL\n");
    return -1;
  }

  /* Inicializar temporizador para mantener timeout si se reciben tramas con
     tipo incorrecto. */
  timerms_t timer;
  timerms_reset(&timer, timeout);

  int frame_len;
  unsigned char eth_buffer[ETH_FRAME_MAX_SIZE];
  struct eth_frame * eth_frame_ptr = NULL;
  int is_target_type;
  int is_my_mac;

  do {
    long int time_left = timerms_left(&timer);

    /* Recibir trama del interfaz Ethernet y procesar errores */
    frame_len = rawnet_recv (iface->raw_iface, eth_buffer, ETH_FRAME_MAX_SIZE, 
                             time_left);
    if (frame_len == -1) {
      fprintf(stderr, "eth_recv(): ERROR en rawnet_recv(): %s\n", 
              rawnet_strerror());
      return -1;
    } else if (frame_len == 0) {

      /* DEBUG */
      if (ETH_LOG >= NORMAL_LOG_LEVEL) {
        printf("eth_recv(): ¡ Timeout !\n");
      }

      return 0;
    }

    /* Comprobar si es la trama que estamos buscando */
    eth_frame_ptr = (struct eth_frame *) eth_buffer;
	is_my_mac = ((memcmp(eth_frame_ptr->dest_addr, 
                        iface->mac_address, MAC_ADDR_SIZE) == 0)) || 
						(((memcmp(eth_frame_ptr->dest_addr, 
                        BROADCAST_MAC_ADDR, MAC_ADDR_SIZE) == 0) &&
						(memcmp(eth_frame_ptr->dest_addr, 
                        iface->mac_address, MAC_ADDR_SIZE) != 0)));
    is_target_type = (ntohs(eth_frame_ptr->type) == type);

  } while ( ! (is_my_mac && is_target_type) );
  
  /* Trama recibida con 'tipo' indicado. Copiar datos y dirección MAC origen */
  memcpy(src, eth_frame_ptr->src_addr, MAC_ADDR_SIZE);
  payload_len = frame_len - ETH_HEADER_SIZE;
  memcpy(buffer, eth_frame_ptr->payload, payload_len);


  /* DEBUG */
  if (ETH_LOG >= VERBOSE_LOG_LEVEL) {
    char mac_addr_str[MAC_ADDR_STR_LENGTH];
    eth_mac_str(src, mac_addr_str);
    char* iface_name = eth_getname(iface);
    printf("eth_recv(type=0x%04x, payload[%d]) < %s/%s\n",
           type, payload_len, iface_name, mac_addr_str);  
    if (ETH_LOG >= DEBUG_LOG_LEVEL) {
      print_pkt((unsigned char *) eth_frame_ptr, frame_len, ETH_HEADER_SIZE);
    }
  }
  
  return payload_len;
}


/* int eth_poll 
 * ( eth_iface_t * ifaces[], int ifnum, long int timeout );
 *
 * DESCRIPCIÓN:
 *   Esta función permite esperar paquetes en múltiples interfaces Ethernet
 *   simultaneamente. Cuando alguna de las interfaces indicadas reciba una
 *   trama, la función devolverá la primera interfaz que tiene una trama
 *   listo para ser recibida mediante la funcion 'eth_recv()'.
 *
 *   Esta operación puede escuchar de los interfaces Ethernet indefinidamente
 *   o un tiempo limitado dependiento del parámetro 'timeout'.
 *
 * PARÁMETROS:
 *  'ifaces': Array con los manejadores de interfaces Ethernet por los que se
 *            quiere recibir.
 *            Todos los interfaces deben haber sido inicializados con
 *            'eth_open()' previamente.
 *   'ifnum': Número de interfaces que aparecen en el array 'ifaces'.
 * 'timeout': Tiempo en milisegundos que debe esperarse a recibir una trama
 *            antes de retornar. Un número negativo indicará que debe
 *            esperarse indefinidamente.
 *
 * VALOR DEVUELTO:
 *   El índice del primer interfaz [0, ifnum-1] que tiene una trama lista para
 *   ser recibida o '-2' si ha expirado el temporizador.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error.
 */
int eth_poll 
( eth_iface_t * ifaces[], int ifnum, long int timeout )
{
  int iface_index;
    
  /* Crear lista de interfaces hardware */
  rawiface_t * raw_ifaces[ifnum];
  int i;
  for (i=0; i<ifnum; i++) {
    raw_ifaces[i] = ifaces[i]->raw_iface;
  }

  /* Llamar a rawnet_poll() y procesar errores */
  iface_index = rawnet_poll(raw_ifaces, ifnum, timeout);
  if (iface_index == -1) {
    fprintf(stderr, "eth_poll(): ERROR en rawnet_poll(): %s\n", 
            rawnet_strerror());
    return -1;
  } else if (iface_index == -2) {

    /* DEBUG */
    if (ETH_LOG >= NORMAL_LOG_LEVEL) {
      printf("eth_poll(): ¡ Timeout !\n");
    }

    return -2;
  }

  /* DEBUG */
  if (ETH_LOG >= DEBUG_LOG_LEVEL) {
    char * iface_name = eth_getname(ifaces[iface_index]);
    printf("eth_poll()=\"%s\"\n", iface_name);
  }

  return iface_index;
}


/* int eth_close ( eth_iface_t * iface );
 * 
 * DESCRIPCIÓN:
 *   Esta función cierra la interfaz Ethernet especificada y libera la memoria
 *   de su manejador.
 *
 * PARÁMETROS:
 *   'iface': Manejador de la interfaz Ethernet que se desea cerrar.
 *
 * VALOR DEVUELTO:
 *   Devuelve 0 si la interfaz Ethernet se ha cerrado correctamente.
 * 
 * ERRORES:
 *   La función devuelve '-1' si se ha producido algún error. 
 */
int eth_close ( eth_iface_t * iface )
{
  int err = -1;

  if (iface != NULL) {

    /* DEBUG */
    if (ETH_LOG >= DEBUG_LOG_LEVEL) {
      char* iface_name = eth_getname(iface);
      printf("eth_close(%s)\n", iface_name);
    }

    err = rawiface_close(iface->raw_iface);
    free(iface);
  }

  return err;
}


/* void eth_mac_str ( mac_addr_t addr, char str[] );
 *
 * DESCRIPCIÓN:
 *   Esta función genera una cadena de texto que representa la dirección MAC
 *   indicada.
 *
 * PARÁMETROS:
 *   'addr': La dirección MAC que se quiere representar textualente.
 *    'str': Memoria donde se desea almacenar la cadena de texto generada.
 *           Deben reservarse al menos 'MAC_ADDR_STR_LENGTH' bytes.
 */
void eth_mac_str ( mac_addr_t addr, char str[] )
{
  if (str != NULL) {
    sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
            addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
  }
}

/* int eth_str_mac ( char* str, mac_addr_t addr );
 *
 * DESCRIPCIÓN:
 *   Esta función analiza una cadena de texto en busca de una dirección MAC.
 *
 * PARÁMETROS:
 *    'str': La cadena de texto que se desea procesar.
 *   'addr': Memoria donde se almacena la dirección MAC encontrada.
 *
 * VALOR DEVUELTO:
 *   Se devuelve 0 si la cadena de texto representaba una dirección MAC.
 *
 * ERRORES:
 *   La función devuelve -1 si la cadena de texto no representaba una
 *   dirección MAC.
 */
int eth_str_mac ( char* str, mac_addr_t addr )
{
  int err = -1;

  if (str != NULL) {
    unsigned int addr_int[MAC_ADDR_SIZE];
    int len = sscanf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
                     &addr_int[0], &addr_int[1], &addr_int[2],
                     &addr_int[3], &addr_int[4], &addr_int[5]);

    if (len == MAC_ADDR_SIZE) {
      int i;
      for (i=0; i<MAC_ADDR_SIZE; i++) {
        addr[i] = (unsigned char) addr_int[i];
      }      
      err = 0;
    }
  }
  
  return err;
}
