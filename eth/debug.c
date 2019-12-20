#include "debug.h"

#include <stdio.h>
#include <stdlib.h>

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
void print_pkt ( unsigned char * packet, int pkt_len, int hdr_len )
{
  if ((packet == NULL) || (pkt_len <= 0)) {
    return;
  }

  int i;
  for (i=0; i<pkt_len; i++) {

    if ((i % 8) == 0) {
      if (i > 0) {
        /* Insert new line char */
        printf("\n");

        /* Change to normal color for index */
        if (i <= hdr_len) {
          printf("\033[0m");
        }
      }

      /* Print hex byte index at the beginning of each line */
      printf("  0x%04x:", i);
      
      if (i < hdr_len)  {
        /* Print initial bytes with a different color */
        printf("\033[1;34m");
      }

    } else if ((i % 4) == 0) {
      /* Print separator between two pairs of 4 bytes */
      printf(" ");
    } 

    /* Back to normal when 'hdr_len' finishes */
    if (i == hdr_len) {
      printf("\033[0m");
    }

    /* Print packet bytes in hexadecimal */
    printf(" %02x", packet[i]);
  }

  /* All packet is header, no color reset yet */
  if (pkt_len == hdr_len) {
    printf("\033[0m");
  }
  
  printf("\n");
}
