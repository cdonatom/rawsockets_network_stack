/* Copyright (C) 2010 Manuel Urueña <muruenya@it.uc3m.es>
 *
 * This file is part of librawnet.
 *
 * Librawnet is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Librawnet is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with librawnet.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TIMERMS_H
#define _TIMERMS_H

/* Estructura opaca que almacena el estado de un temporizador con precisión de
 * milisegundos.
 *
 * Nunca acceda directamente a los campos de esta estructura, en su lugar
 * utilice las funciones proporcionadas por la librería.
 */
struct timerms {
  long long int reset_timestamp;
  long long int timeout_timestamp;
};

typedef struct timerms timerms_t;


/* long long int timerms_time();
 * 
 * DESCRIPCIÓN:
 *   Esta función devuelve el número de milisegundos que han pasado desde el 1
 *   de enero de 1970 hasta el momento actual.
 *
 * VALOR DEVUELTO:
 *   El tiempo actual, medido en milisegundos
 *
 * ERRORES:
 *   La función devuelve -1 si se ha producido un error.
 */
long long int timerms_time();


/* long long int timerms_reset ( timerms_t * timer, long int timeout )
 *
 * DESCRIPCIÓN: 
 *   Esta función inicia un temporizador de 'timeout' milisegundos de
 *   duración. Es posible establecer un temporizador infinito, que no vence
 *   nunca, si se especifica un 'timeout' negativo.
 *
 * PARÁMETROS:
 *     'timer': El temporizador que se desea establecer.
 *              La memoria de la estructura 'timerms_t' debe haber sido
 *              reservada previamente.
 *   'timeout': Tiempo en milisegundos que se desea que dure el temporizador
 *              antes de que expire. Un tiempo negativo generará un
 *              temporizador infinito, que no expirará nunca.
 *
 * VALOR DEVUELTO:
 *   El tiempo actual, medido en milisegundos.
 *
 * ERRORES:
 *   La función devuelve '-1' si se ha producido un error al obtener el tiempo
 *   actual o si el 'timer' es 'NULL'.
 */
long long int timerms_reset ( timerms_t * timer, long int timeout );


/* long timerms_elapsed ( timerms_t * timer );
 *
 * DESCRIPCIÓN: 
 *   Esta función devuelve el tiempo que ha pasado desde la inicialización del
 *   temporizador.
 *
 * PARÁMETROS:
 *   'timer': El temporizador que se quiere consultar. 
 *            Dicho temporizador debe haber sido inicializado con
 *            'timerms_reset()' previamente.
 *
 * VALOR DEVUELTO: 
 *   Devuelve el número de milisegundos que han pasado desde que el
 *   temporizador fue inicializado (independientemente de que ya haya expirado
 *   o no).
 *
 * ERRORES:
 *   La función devuelve '-1' si el temporizador no se ha inicializado
 *   corectamente.
 */
long int timerms_elapsed ( timerms_t * timer );


/* long int timerms_left ( timerms_t * timer )
 *
 * DESCRIPCIÓN: 
 *   Esta función devuelve el tiempo que le queda al temporizador antes de que
 *   expire. Un tiempo cero indicará que el temporizador ya ha expirado, y un
 *   tiempo negativo que el temporizador es infinito y no expirará nunca.
 *
 * PARÁMETROS:
 *   'timer': El temporizador que se quiere consultar. 
 *            Dicho temporizador debe haber sido inicializado con
 *            'timerms_reset()' previamente.
 *
 * VALOR DEVUELTO: 
 *    Devuelve el número de milisegundos que quedan antes de que el
 *    temporizador expire, o 0 si ya ha expirado. Un tiempo negativo (distinto
 *    a -1) indica que el temporizador es infinito y no expirará nunca.
 *
 * ERRORES:
 *   La función devuelve '-1' si el temporizador no se ha inicializado
 *   corectamente.
 */
long int timerms_left ( timerms_t * timer );


#endif /* #ifndef _TIMERMS_H_ */
