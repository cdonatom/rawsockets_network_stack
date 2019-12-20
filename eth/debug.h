#ifndef _DEBUG_H
#define _DEBUG_H

#define   QUIET_LOG_LEVEL 0
#define  NORMAL_LOG_LEVEL 1
#define VERBOSE_LOG_LEVEL 2
#define   DEBUG_LOG_LEVEL 3

/* void print_pkt ( unsigned char * packet, int pkt_len, int hdr_len );
 * 
 * DESCRIPCIÓN:
 *   Esta función permite imprimir por salida estándar los contenidos del
 *   paquete especificado. Además, los primeros 'hdr_len' bytes del mensaje se
 *   resaltarán con otro color para facilitar su visualización.
 *
 * PARÁMETROS:
 *     'packet': Puntero al contenido del paquete que se quiere imprimir.
 *    'pkt_len': Longitud total en bytes del paquete a imprimir.
 *    'hdr_len': Número de bytes iniciales que se quieren resaltar,
 *               imprimiendolos en otro color. Utilice cualquier valor menor o
 *               igual a cero para no utilizar esta característica.
 */
void print_pkt ( unsigned char * packet, int pkt_len, int hdr_len );


#endif /* _DEBUG_H */
