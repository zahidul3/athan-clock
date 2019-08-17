/*
 * uart_debug.h
 *
 *  Created on: Jul 31, 2019
 *      Author: Jahid's Desktop
 */

#ifndef UART_DEBUG_H_
#define UART_DEBUG_H_

#define DEBUG_BUFFER_SIZE 32

void printDebug(char * debugString);
void initUART0Debug(void);

#endif /* UART_DEBUG_H_ */
